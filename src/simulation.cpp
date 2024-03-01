#include "concolic.h"
#include "globals.h"
#include "smt_lib.h"
#include "state.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <list>
#include <cstdlib>
#include <string>
#include <fstream>
#include <set>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <sys/stat.h>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm> 
#include <random>

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <Python.h>

using namespace std;


// store the blocks
std::unordered_set<unsigned int> prev_ids;
std::unordered_set<unsigned int> curr_ids;

//configuration
const bool		enable_error_check = false;
const bool		enable_obs_padding = true;
const bool		enable_sim_copy = false;
const bool		enable_yices_debug = true;
const uint 		iteration_limit = 1000;//for one branch, this is the maximum number of iterations
const uint      total_limit = 10;
const uint      learning_limit = 3;
unordered_map<SMTBasicBlock*, uint> iter_count;

static inline void free_stack();
context_t *yices_context;
static vector<constraint_t*> constraints_stack;
static char sim_file_name[64];
//static char cnst_file_name[64];
static char mem_file_name[64];
static uint hash_table[1024*16];
static uint sim_num;


static SMTBranch* selected_branch;
static uint selected_clock;

static void check_satisfiability();

// 选择动作 best_ation
// std::string chooseAction(const State& state, const std::unordered_map<std::string, std::vector<double>>& q_table, double epsilon, const std::vector<std::string>& actions) {
//     std::string stateKey = state.toString();

//     // ε-贪婪策略
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_real_distribution<> dis(0, 1);
//     if (dis(gen) < epsilon) {
//         std::uniform_int_distribution<> actionDis(0, actions.size() - 1);
//         std::cout << "Chosen Action: " << actions[actionDis(gen)] << " - Reason: Exploration (random choice)" << std::endl;
//         return actions[actionDis(gen)]; // 探索
//     } else {
//         auto it = std::find_if(q_table.begin(), q_table.end(), [&](const auto& pair) {
//             return pair.first == stateKey;
//         });
//         if (it != q_table.end()) {
//             auto maxIt = std::max_element(it->second.begin(), it->second.end());
//             std::cout << "Chosen Action: " << actions[std::distance(it->second.begin(), maxIt)] << " - Reason: Exploitation (best current option)" << std::endl;
//             return actions[std::distance(it->second.begin(), maxIt)]; // 利用
//         }
//         return actions[0]; // 默认动作
//     }
// }

void run_python_init() {
    // 初始化Python解释器
    Py_Initialize();

    // 设置Python搜索路径
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('.')"); // 添加当前目录到Python路径

	// 初始化函数
	 PyRun_SimpleString(R"(
import random
import gym
import numpy as np
import collections
from tqdm import tqdm
import torch
import torch.nn.functional as F
import matplotlib.pyplot as plt
import gym
from gym import spaces
import numpy as np

#单周期数据处理
def parse_sim_log(file_path):
    # 初始化数据结构
    cycles_data = {}

    with open(file_path, 'r') as file:
        for line in file:
                line = line.strip()
                parts = line.split(':')
                register_name = parts[0].strip()
                register_value = parts[1].strip()
                cycles_data[register_name] = register_value

    return cycles_data



def convert_values_to_int(data):
    for key, value in data.items():
        data[key]= int(value)
    return data 

def get_state(file_path):
    cycles_data = parse_sim_log(file_path)
    states_list = list(cycles_data.items())
    state_dict = dict(states_list[2:])
    return convert_values_to_int(state_dict)

def get_action(file_path):
    cycles_data = parse_sim_log(file_path)
    return int(cycles_data['Action'])

def get_reward(file_path):
    cycles_data = parse_sim_log(file_path)
    return float(cycles_data['reward'])

class ReplayBuffer:
    ''' 经验回放池 '''
    def __init__(self, capacity):
        self.buffer = collections.deque(maxlen=capacity)  # 队列,先进先出

    def add(self, state, action, reward, next_state, done):  # 将数据加入buffer
        self.buffer.append((state, action, reward, next_state, done))

    def sample(self, batch_size):  # 从buffer中采样数据,数量为batch_size
        transitions = random.sample(self.buffer, batch_size)
        state, action, reward, next_state, done = zip(*transitions)
        return np.array(state), action, reward, np.array(next_state), done

    def size(self):  # 目前buffer中数据的数量
        return len(self.buffer)

class Qnet(torch.nn.Module):
    def __init__(self, state_dim, hidden_dim, action_dim):
        super(Qnet, self).__init__()
        # 定义四个全连接层
        self.fc1 = torch.nn.Linear(state_dim, 32)
        self.fc2 = torch.nn.Linear(32, 16)
        self.fc3 = torch.nn.Linear(16, 8)
        self.fc4 = torch.nn.Linear(8, action_dim)
    
    def forward(self, x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = F.relu(self.fc3(x))
        x = self.fc4(x)
        return F.softmax(x, dim=1)  # 使用softmax输出每个动作的概率

class DQN:
    ''' DQN算法 '''
    def __init__(self, state_dim, hidden_dim, action_dim, learning_rate, gamma,
                 epsilon, target_update, device):
        self.action_dim = action_dim
        # Q网络
        self.q_net = Qnet(state_dim, hidden_dim,
                          self.action_dim).to(device) 
        # 目标网络
        self.target_q_net = Qnet(state_dim, hidden_dim,
                                 self.action_dim).to(device)
        # 使用Adam优化器
        self.optimizer = torch.optim.Adam(self.q_net.parameters(),
                                          lr=learning_rate)
        self.gamma = gamma  # 折扣因子
        self.epsilon = epsilon  # epsilon-贪婪策略
        self.target_update = target_update  # 目标网络更新频率
        self.count = 0  # 计数器,记录更新次数
        self.device = device

    
    def take_action(self, state, branch_list):
        state_tensor = torch.tensor([state], dtype=torch.float).to(self.device)
         if np.random.random() < self.epsilon:
        # 随机选择一个动作
            return [random.choice(branch_list)]
        with torch.no_grad():  # 在进行推断时不需要计算梯度
            action_probs = self.q_net(state_tensor)
            action_probs = action_probs.cpu().numpy()  # 转换为numpy数组，假设你的模型输出的是概率
        # 获取按概率排序的动作索引
        sorted_indices = np.argsort(action_probs[0])[::-1]
        # 根据排序后的索引获取对应的branch_list
        sorted_branch_list = [branch_list[i] for i in sorted_indices]

        return sorted_branch_list



    def update(self, transition_dict):
        states = torch.tensor(transition_dict['states'],
                              dtype=torch.float).to(self.device)
        actions = torch.tensor(transition_dict['actions']).view(-1, 1).to(
            self.device)
        rewards = torch.tensor(transition_dict['rewards'],
                               dtype=torch.float).view(-1, 1).to(self.device)
        next_states = torch.tensor(transition_dict['next_states'],
                                   dtype=torch.float).to(self.device)
        dones = torch.tensor(transition_dict['dones'],
                             dtype=torch.float).view(-1, 1).to(self.device)
        
        q_values = self.q_net(states).gather(1, actions)  # Q值
        # 下个状态的最大Q值
        max_next_q_values = self.target_q_net(next_states).max(1)[0].view(
            -1, 1)
        q_targets = rewards + self.gamma * max_next_q_values * (1 - dones
                                                                )  # TD误差目标
        dqn_loss = torch.mean(F.mse_loss(q_values, q_targets))  # 均方误差损失函数
        self.optimizer.zero_grad()  # PyTorch中默认梯度会累积,这里需要显式将梯度置为0
        dqn_loss.backward()  # 反向传播更新参数
        self.optimizer.step()

        if self.count % self.target_update == 0:
            self.target_q_net.load_state_dict(
                self.q_net.state_dict())  # 更新目标网络
        self.count += 1



class RegistersEnv(gym.Env):
    """基于寄存器状态的自定义环境"""
    def __init__(self, states):
        super(RegistersEnv, self).__init__()
        self.states = states  # 将states作为初始化参数
        
        # 动态构建观察空间
        self.observation_space = self.create_observation_space()

        # 定义动作空间，假设有67个分支可以选择
        self.action_space = spaces.Discrete(347)

        # 初始化状态
        self.reset()
        
    def create_observation_space(self):
        space_dict = {}
        for key, value in self.state.items():
            if key in space_dict:
                # 更新最大值，如果当前值更大
                space_dict[key].n = max(space_dict[key].n, value + 1)
            else:
                # 为新键创建一个空间
                space_dict[key] = spaces.Discrete(value + 1)
        return spaces.Dict(space_dict)

    def step(self, action):
        # 状态更新逻辑
        # 随机更新
        self.state = random.choice(self.states)
        reward = np.random.rand()
        done = np.random.choice([True, False])
        return self.state, reward, done, {}

    def step_sim(self, action, next_state,reward):
        self.state = next_state
        self.reward = reward 
        return self.state, self.reward

    def render(self, mode='human'):
        print(f"当前状态: {self.state}")

# DQN参数
learning_rate = 0.001
gamma = 0.99
epsilon = 0.1
target_update = 10
capacity = 10000
batch_size = 16
episodes = 500   # 总迭代轮次
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
done = False

#初始化环境
env = RegistersEnv(state_int)

# 环境参数
branch_list = range(0,347)
state_dim = len(state_int)  # 状态维度
action_dim = len(branch_list) # 动作维度
hidden_dim = 32  # 隐藏层维数

# 初始化DQN代理和回放缓冲区
dqn_agent = DQN(state_dim, hidden_dim, action_dim, learning_rate, gamma, epsilon, target_update, device)
replay_buffer = ReplayBuffer(capacity)
	
	
)");
}
void execute_python_sort_branch() {
PyRun_SimpleString(R"(
    state = get_state("data.state")
    state = np.array(list(state_int.values()))  # 将状态字典转换为数组
    sorted_branch_list = dqn_agent.take_action(state, list(range(0,347)))
    with open('sorted_branch_list.txt', 'w') as file:
        file.write(f"{sorted_branch_list}\n") 
)");
}

void execute_python_update_DQN() {
PyRun_SimpleString(R"(
	action = get_action("data.state")
	reward = get_reward("data.state")
	next_state = get_state("data.state")
	next_state = np.array(list(next_state.values()))  # 将下一个状态字典转换为数组
	replay_buffer.add(state, action, reward, next_state, done)  # 保存到回放缓冲区
	# 检查回放缓冲区是否足够大以开始学习
	if replay_buffer.size() > batch_size:
		transitions = replay_buffer.sample(batch_size)  # 从回放缓冲区采样
		dqn_agent.update({
			'states': np.vstack(transitions[0]),
			'actions': transitions[1],
			'rewards': transitions[2],
			'next_states': np.vstack(transitions[3]),
			'dones': transitions[4]
		})
	state = next_state  # 更新状态
)");
}

// 修改chooseAction函数，返回一个动作列表
std::vector<unsigned int> chooseActions(const State& state, const std::unordered_map<std::string, std::vector<double>>& q_table, double epsilon, const std::vector<unsigned int>& actions) {
    std::string stateKey = state.toString();
    std::vector<unsigned int> actionList;
    // ε-贪婪策略
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    if (dis(gen) < epsilon) {
        // 以随机顺序返回动作列表作为探索
        actionList = actions;
        std::shuffle(actionList.begin(), actionList.end(), gen);
        std::cout << "Action List: Exploration (random order)" << std::endl;
    } else {
        // 根据Q值排序返回动作列表
        auto it = q_table.find(stateKey);
        if (it != q_table.end()) {
            // 创建一个包含动作索引和对应Q值的向量
            std::vector<std::pair<size_t, double>> indexedQValues;
            for (size_t i = 0; i < it->second.size(); ++i) {
                indexedQValues.emplace_back(i, it->second[i]);
            }
            // 根据Q值降序排序
            std::sort(indexedQValues.begin(), indexedQValues.end(), [](const auto& a, const auto& b) {
                return a.second > b.second;
            });
            // 填充动作列表，按Q值排序
            for (const auto& iq : indexedQValues) {
                actionList.push_back(actions[iq.first]);
            }
            std::cout << "Action List: Exploitation (sorted by Q values)" << std::endl;
        } else {
            // 如果状态不在Q表中，返回默认动作列表
            actionList = actions;
        }
    }
    return actionList;
}




// 更新Q值，action代表被选择动作的索引
void updateQValue(std::unordered_map<std::string, std::vector<double>>& q_table, const State& prevState, const State& newState, const br_cnst_t* branch, double reward, double alpha, double gamma, size_t actionSize) {
    unsigned int actionIndex = branch->br->id;
	unsigned int clk = branch->cnst->clock;
	std::string prevStateKey = prevState.toString();
    std::string newStateKey = newState.toString();
	std::cout << "Choose branch: " << actionIndex << "  Clock: "<< clk << std::endl;
    if (q_table.find(prevStateKey) == q_table.end()) {
        q_table[prevStateKey] = std::vector<double>(actionSize, 0.0);
    }
    if (q_table.find(newStateKey) == q_table.end()) {
        q_table[newStateKey] = std::vector<double>(actionSize, 0.0);
    }

    auto maxIt = std::max_element(q_table[newStateKey].begin(), q_table[newStateKey].end());
    double maxQValueNewState = *maxIt;

    q_table[prevStateKey][actionIndex] += alpha * (reward + gamma * maxQValueNewState - q_table[prevStateKey][actionIndex]);
}



double calculateReward(bool foundNewNode) {
    double reward = 0.0;
    // 如果发现了新的节点，给予奖励
    if (foundNewNode) {
        reward = 1.0; 
    }
	else{
		reward = - 0.1;
	}

    return reward;
}


void sortBranches(std::vector<br_cnst_t*>& branches, const std::vector<unsigned int>& action_list) {
    // 首先，创建一个动作ID到排序索引的映射
    std::unordered_map<unsigned int, size_t> action_order;
    for (size_t i = 0; i < action_list.size(); ++i) {
        action_order[action_list[i]] = i;
    }

    // 使用自定义的比较函数进行排序
    std::sort(branches.begin(), branches.end(),
              [&action_order](const br_cnst_t* a, const br_cnst_t* b) -> bool {
                  // 获取两个分支的动作ID
                  unsigned int actionA = a->br->id;
                  unsigned int actionB = b->br->id;

                  // 比较它们在action_list中的位置
                  return action_order[actionA] < action_order[actionB];
              });
}

// 从指定的文件中读取动作列表，并返回一个包含这些动作的向量
std::vector<unsigned int> readActionsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<unsigned int> action_list;
    if (!file.is_open()) {
        std::cerr << "Error opening file: " + filename << std::endl;
        return action_list; // 返回一个空的向量
    }
    unsigned int action;
    while (file >> action) {
        action_list.push_back(action);
    }
    file.close(); // 关闭文件
    return action_list;
}

void writeLastFContentToFile(const std::string& inputFileName, FILE* outputFile) {
    std::ifstream inputFile(inputFileName);
    std::string line;
    std::string lastFContent;

    if (inputFile.is_open()) {
        while (std::getline(inputFile, line)) {
            if (line.find(";F") != std::string::npos) {
                lastFContent = line.substr(line.find(";F") + 3);  // Extract content after ";F"
            }
        }
        inputFile.close();

        if (outputFile != nullptr) {
            fprintf(outputFile, "%s\n", lastFContent.c_str());
            std::cout << "Content written to file." << std::endl;
        } else {
            std::cout << "Invalid output file." << std::endl;
        }
    } else {
        std::cout << "Failed to open input file: " << inputFileName << std::endl;
    }
}

static inline void compile() {
    string cmd = "iverilog -o conc_run.vvp " + string(g_output_file) + \
		  " " + string(g_tb_file);
	printf("%s\n", cmd.c_str());
	system(cmd.c_str());
}

bool file_exist (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

static constraint_t* create_clock(uint clock){
	constraint_t* cnst = new constraint_t;
	cnst->type = CNST_CLK;
	cnst->clock = clock;
	cnst->obj = NULL;
    cnst->hash_value = 0;
	cnst->index = 0;
	cnst->yices_term = -1;
    return cnst;
}

static set<uint> path_hash_map;
static uint g_hash_value;
static void update_path_hash_map(constraint_t* cnst){
    cnst->hash_value = g_hash_value;
    g_hash_value += hash_table[cnst->obj->id % 16384]*cnst->index;
    path_hash_map.insert(g_hash_value);
}

static void update_vvp(uint target_cycles = g_step) {
    std::ifstream vvp("conc_run.vvp");
    if (!vvp.is_open()) {
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    while (getline(vvp, line)) {
        lines.push_back(line);
    }
    vvp.close(); // 关闭文件以便稍后覆盖

    std::string pattern_str1 = "_conc_ram";
    std::string pattern_str2 = "delay "; // Only for test
    for (auto& _line : lines) {
        if (_line.find(pattern_str1) != std::string::npos) {
            char* cline = strdup(_line.c_str());
            char* compos_p1 = strtok(cline, " ");
            bool pre_target = false;
            std::string newLine;
            while (compos_p1 != NULL) {
                if (pre_target) {
                    pattern_str2 += std::string(compos_p1);
                    newLine += std::to_string(target_cycles) + " ";
                    pre_target = false;
                } else {
                    newLine += compos_p1 + std::string(" ");
                }
                if (std::string(compos_p1) == "\"_conc_ram\",") {
                    pre_target = true;
                }
                compos_p1 = strtok(NULL, " ");
            }
            _line = newLine;
            free(cline);
        } else if (_line.find(pattern_str2) != std::string::npos) {
            std::string target_str = std::to_string(g_unroll * 10);
            int pos_start = _line.find(target_str);
            _line = _line.substr(0, pos_start) + std::to_string(g_unroll * 10) + _line.substr(pos_start + target_str.length());
        }
        // 不需要额外的else分支，因为_line已经包含了未修改的内容
    }

    // 将修改后的内容写回原文件
    std::ofstream vvp_out("conc_run.vvp", std::ofstream::trunc);
    for (const auto& _line : lines) {
        vvp_out << _line << '\n';
    }
    vvp_out.close();
}


static void smt_yices_dump_error(){
    //reset context
    yices_reset_context(yices_context);

    //insert initial assertion to zero for registers
    SMTSigCore::yices_insert_reg_init(yices_context);

    FILE* f_dbg = fopen("debug.txt", "w");
    
    //dump yices constraints
    for(auto it:constraints_stack){
		if(it->type != CNST_CLK){
            fprintf(f_dbg, "[%3u]    ", it->obj->id);
            yices_pp_term(f_dbg, it->yices_term, 1000, 1, 0);
            yices_assert_formula(yices_context, it->yices_term);
            if(yices_check_context(yices_context, NULL) == STATUS_UNSAT){
                break;
            }
		}
	}
    
    fclose(f_dbg);
}

static constraint_t* create_constraint(uint clock, SMTAssign* assign){
	constraint_t* cnst = new constraint_t;
	cnst->clock = clock;
	cnst->obj = assign;
	cnst->index = constraints_stack.size();
    cnst->yices_term = assign->update_term();
    if(cnst->yices_term <= 0){
        yices_print_error(stdout);
        error("Term evaluation failed at assign id: %u", assign->id);
    }
    
	if(assign->assign_type == SMT_ASSIGN_BRANCH){
		cnst->type = CNST_BRANCH;
		SMTBranch* br = dynamic_cast<SMTBranch*>(assign);
		assert(br);
        update_path_hash_map(cnst);
    	br->set_covered_clk(sim_num, clock);
	} else{
        cnst->type = CNST_ASSIGN;
		assign->set_covered(sim_num);
    }
    return cnst;
}

static void write_first_clock(const char* file_name){
	FILE* f_test = fopen(file_name, "r");
	if(f_test == NULL){
		perror("Error opening file!");
		return;
	}
	// Get the size of file
	fseek(f_test, 0, SEEK_END);
	long f_size = ftell(f_test);
	fseek(f_test, 0, SEEK_SET);

	// Read the content to buffer
	char* buffer = (char*)malloc(f_size + 1);
	fread(buffer, f_size, 1, f_test);
	buffer[f_size] = '\0';
	fclose(f_test);

	// Write the first clock and the rest
	f_test = fopen(file_name, "w");
	if (f_test == NULL) {
    	perror("Error opening file");
    	free(buffer);
    	return;
	}
	fprintf(f_test, ";_C%11u\n", 0);
	fprintf(f_test, "%s", buffer);
	free(buffer);
	fclose(f_test);
}


static void build_stack(uint sim_clk=g_step) {
	FILE* f_test = NULL;
	if(enable_sim_copy){
		f_test = fopen(sim_file_name, "r");
		write_first_clock(sim_file_name);
	} else{
		f_test = fopen("sim.log", "r");
		write_first_clock("sim.log");
	}
	assert(f_test);
	uint clock = 0;
    g_hash_value = 0;
	char tag[16];
	uint val;
	
	g_is_new_block = false;

	//constraints_stack.push_back(create_clock(0));
	while(true){
		fscanf(f_test, "%s%u", tag, &val);
		if(strcmp(tag, ";_C") == 0){
			clock = val;
			if(clock == g_sim_clk+1)	
				break;
			constraints_stack.push_back(create_clock(clock));
            SMTSigCore::set_input_version(clock);
            SMTSigCore::commit_versions(clock);
		}
		else if (strcmp(tag, ";A") == 0){
			SMTAssign* assign = SMTAssign::get_assign(val);
			constraints_stack.push_back(create_constraint(clock, assign));
			curr_ids.insert(assign->block->id);
		}
		else if (strcmp(tag, ";R") == 0){
            // 这里添加处理状态的逻辑
            char stateName[256];
            unsigned int stateValue;
            fscanf(f_test, "%s = %u", stateName, &stateValue); // 读取状态名和值
            // 可能需要根据实际情况处理或存储获取到的状态
			SMTState::add_state(stateName, stateValue, clock);
        }
	}


	// examine if cover the new block
	if(!g_is_new_block){
		for(auto id:curr_ids){
			if(prev_ids.find(id) == prev_ids.end()){
				g_is_new_block = true;
				break;
			}
		}
	}

	prev_ids = std::move(curr_ids); // 将curr_ids的内容移动到prev_ids


	// printf("If the new block is covered: %d\n", g_is_new_block);
	fclose(f_test);
}


static inline void free_stack() {
	while(!constraints_stack.empty()){
		delete constraints_stack.back();
		constraints_stack.pop_back();
	}
}

void init_hash_table(){
    for(uint i=0; i< 1024*16; i++){
        hash_table[i] = rand();
    }
}

static void init() {
    compile();
    init_hash_table();
    ctx_config_t *config = yices_new_config();
    yices_default_config_for_logic(config, "QF_BV");
    yices_context = yices_new_context(config);
    yices_free_config(config);
}

/*static void set_sim_log_name(uint sim_num) {
	sprintf(sim_file_name, "tests/sim_%04u.log", sim_num);
}

static void set_cnst_log_name(uint sim_num) {
	sprintf(cnst_file_name, "tests/constraints_%04u.ys", sim_num);
}

static void set_mem_log_name(uint sim_num) {
	sprintf(mem_file_name, "tests/data_%04u.mem", sim_num);
}*/


static void sim() {
	if(enable_sim_copy){
		string cmd = "cp data.mem " + string(mem_file_name);
		system(cmd.c_str());
		cmd = "vvp conc_run.vvp > " + string(sim_file_name);
		system(cmd.c_str());
	} else{
		system("vvp conc_run.vvp > sim.log");
	}
}

static void dump_yices_constraints(uint top_idx) {
	for(uint i=0; i<=top_idx; i++){
		constraint_t* cnst = constraints_stack[i];
		if(cnst->type != CNST_CLK){
            yices_assert_formula(yices_context, cnst->yices_term);
		}
	}
}

static bool check_yices_status(){
	smt_status_t status = yices_check_context(yices_context, NULL);
	if(status == STATUS_SAT){
		return true;
	}
	else if(status == STATUS_UNSAT){
		return false;
	}
	else{
		error("Syntax error in constraints");
		return false;
	}
}

static bool solve_constraints(uint clock) {
	
	bool is_sat = check_yices_status();
	if(is_sat){
		model_t *model = yices_get_model(yices_context, true);
		FILE *f_out = fopen("model.log", "w");
		yices_print_model(f_out, model);
		// yices_print_model(stdout, model);
		fclose(f_out);
		g_data_step.update_and_dump("model.log", g_data_mem, clock);
		yices_free_model(model);
	}
	return is_sat;
}

static uint call_to_solver = 0;

bool compare_dist(br_cnst_t* a, br_cnst_t* b){
    if(a->br->block->distance == b->br->block->distance){
        return a->cnst->clock < b->cnst->clock;
    }
	return a->br->block->distance < b->br->block->distance;
}

bool compare_prob(br_cnst_t* a, br_cnst_t* b){
	if(a->br->branch_probability == b->br->branch_probability){
		return a->cnst->clock < b->cnst->clock;
	}
	return a->br->branch_probability > b->br->branch_probability;
}

static void get_available_branches(vector<br_cnst_t*>& branches, uint begin_clk, uint end_clk){
	vector<br_cnst_t*> temp;
	for(auto it:branches){
		if(it->cnst->clock >= begin_clk && it->cnst->clock <= end_clk){
			temp.push_back(it);
		}
	}
	branches = temp;
}

static void update_path_taken(br_cnst_t* alt_path){
    uint alt_path_hash = alt_path->cnst->hash_value + hash_table[alt_path->br->id % 16384]*alt_path->cnst->index;
	path_hash_map.insert(alt_path_hash);
}

static bool is_path_taken(br_cnst_t* alt_path){
    uint alt_path_hash = alt_path->cnst->hash_value + hash_table[alt_path->br->id % 16384]*alt_path->cnst->index;
    return path_hash_map.find(alt_path_hash) != path_hash_map.end();
}




void simulate_build_stack() {
	//simulate
	sim();
	//reset variable versions to zero
	SMTSigCore::clear_all_versions();
	//build constraints stack
	constraints_stack.clear();
	build_stack(g_sim_clk);
}


void explore_one_step(SMTPath* curr_path) {
	SMTPath* step_path = NULL;

	// generate random inputs and add to data
	g_data_step.generate(g_data_mem_step);
	step_path = new SMTPath(g_data_step);
	curr_path->ConnectPath(step_path);

	// dump path to file
	curr_path->Dump(g_data_mem);

	// get the simulation clk and build the stack
	update_vvp(curr_path->data.get_clk());
	g_sim_clk = curr_path->data.get_clk();
	simulate_build_stack();  
}

static bool find_next_cfg(SMTPath* path, uint init_clk, uint curr_clk) {
	uint begin_clk = init_clk;
	uint end_clk = curr_clk;

	const uint size = constraints_stack.size();
	vector<br_cnst_t*> branches;
	
	//create branches
	uint idx = 0;
	while(constraints_stack[idx]->clock == 0){
		idx++;
	}
	for(; idx < size; idx++){
		constraint_t* cnst = constraints_stack[idx];
		if(cnst->type == CNST_BRANCH){
			SMTBranch* br = dynamic_cast<SMTBranch*>(cnst->obj);
			assert(br);
            if(!br->is_dep){
                continue;
            }
			
			//add all selectable branches for sorting
			SMTBranchNode* node = br->parent_node;
			for(auto it:node->branch_list){
				//Yangdi: dont add if the smallest too far away
				if((it != br)){
				//if(it != br){
					br_cnst_t* alt_br = new br_cnst_t;
					alt_br->br = it;
					alt_br->cnst = cnst;
                    if(is_path_taken(alt_br) == false){
                        branches.push_back(alt_br);
                    }
				}
			}
		}
	}
	
	//get the branches between begin and end
	get_available_branches(branches, begin_clk+1, end_clk);

	//choose mutated branch，sort by QLearning
	// std::vector<unsigned int> actions;
	// for(std::vector<br_cnst_t*>::size_type i = 0;i < branches.size();++i){
	// 	actions.push_back(branches[i]->br->id);
	// }
	// State currentState("model.log");//初始状态
	// std::vector<unsigned int> action_list = chooseActions(currentState, q_table, epsilon_qlearn, actions);//action_list = 选择branch index
	// sortBranches(branches,action_list);


	// //choose mutated branch，sort by DQN
	run_python_init();
	execute_python_sort_branch();
	std::vector<unsigned int> action_list = readActionsFromFile("branch_list.txt");
	sortBranches(branches,action_list);

	// //sort by distance
	//sort(branches.begin(), branches.end(), compare_dist);
	//sort by probability
	//sort(branches.begin(), branches.end(), compare_prob);
    
	// if(!user_select_branch)

	for(auto it:branches){
		if (it->cnst == nullptr) {
			std::cerr << "Error: Constraint pointer is null." << std::endl;
			continue; // Skip this iteration
			}
		uint clock = it->cnst->clock;

		yices_print_error(stdout);

		//reset context
		yices_reset_context(yices_context);
		//insert initial assertion to zero for registers
		SMTSigCore::yices_insert_reg_init(yices_context);
		constraint_t** cnst = constraints_stack.data();
		while((*cnst)->clock != clock){
			if((*cnst)->type != CNST_CLK){			
				smt_yices_assert(yices_context, (*cnst)->yices_term, (*cnst)->obj);
			}
			cnst++;
		}	
		assert((*cnst)->type == CNST_CLK);
		cnst++;
		//restore version
		SMTSigCore::restore_versions(clock);
		const SMTProcess* target_process = it->cnst->obj->process;
		while(*cnst != it->cnst){
			const SMTProcess* process = (*cnst)->obj->process;
			if(!process->is_edge_triggered || (process == target_process)){
				term_t term = (*cnst)->obj->update_term();
				smt_yices_assert(yices_context, term, (*cnst)->obj);
			}
			cnst++;
		}


		//add mutated branch
		smt_yices_assert(yices_context, it->br->update_term(), it->br);
		
		g_data_step.clear_input_vector();
		g_data_step.intercept(path->data, 0, clock);
		g_data_step.dump(g_data_mem_step);
		
		smt_yices_dump_error();

		call_to_solver++;
		check_satisfiability();
		if(solve_constraints(clock)){
			selected_branch = it->br;
			selected_clock = clock;
			//insert hash value even if potentially incorrect
			update_path_taken(it);
			if(!enable_error_check){
				it->br->set_covered_clk(sim_num+1, clock);
			}
			g_sim_clk = clock;
			simulate_build_stack();

			// Give the reward
			// // Update Qlearning reward 
			// State newState("model.log"); // 获得新状态
			// double reward = calculateReward(g_is_new_block); // 计算奖励
			// updateQValue(q_table, currentState, newState,it, reward, alpha_qlearn, gamma_qlearn,1);//更新Q表

			//// Update DQN reward 
			double reward = calculateReward(g_is_new_block); // 计算奖励
			unsigned int actionIndex = it->br->id;
			SMTState::print_state_at_clock(g_data_state, g_sim_clk, reward, actionIndex);
			execute_python_update_DQN();

			if(g_is_new_block){
				SMTBranch::increase_probability(selected_branch);
			}else{
				if(sim_num > prob_num){
					SMTBranch::decrease_probability(selected_branch);
				}
			}
			SMTBranch::print_probability();
			selected_branch->k_permit_covered++;
			// Xiangchen: Because we use the probability to sort the branches
			// So we need not to consider the distance
			selected_branch->block->distance++;
			return true;
		}
	}	

	return false;
}
	// }else{
	// 	// 打印所有分支
	// 	std::cout << "Available branches:" << std::endl;
	// 	for(size_t i = 0; i < branches.size(); ++i) {
	// 		std::cout << "Index " << i << ": " << branches[i]->br->print() << std::endl;
	// 	}

	// 	// 获取用户输入
	// 	std::cout << "Enter the index of the branch to select: ";
	// 	size_t user_choice;
	// 	std::cin >> user_choice;

	// 	// 检查用户输入的有效性
	// 	if(user_choice >= branches.size()) {
	// 		std::cerr << "Invalid index selected." << std::endl;
	// 		return false;
	// 	}

	// 	// 使用用户选择的分支
	// 	auto selected_branch_it = branches.begin() + user_choice;

	// 	// 用选择的分支代替原来的自动选择逻辑
	// 	auto it = selected_branch_it;

	// 	//printf("Trying %s\n", it->br->print().c_str());
	// 	uint clock = (*it)->cnst->clock;
	// 	//reset context
	// 	yices_reset_context(yices_context);

	// 	//insert initial assertion to zero for registers
	// 	SMTSigCore::yices_insert_reg_init(yices_context);
		
	// 	constraint_t** cnst = constraints_stack.data();
	// 	while((*cnst)->clock != clock){
	// 		if((*cnst)->type != CNST_CLK){			
	// 			smt_yices_assert(yices_context, (*cnst)->yices_term, (*cnst)->obj);
	// 		}
	// 		cnst++;
	// 	}	
	// 	assert((*cnst)->type == CNST_CLK);
	// 	cnst++;
		
	// 	//restore version
	// 	SMTSigCore::restore_versions(clock);
	// 	const SMTProcess* target_process = (*it)->cnst->obj->process;
	// 	while(*cnst != (*it)->cnst){
	// 		const SMTProcess* process = (*cnst)->obj->process;
	// 		if(!process->is_edge_triggered || (process == target_process)){
	// 			term_t term = (*cnst)->obj->update_term();
	// 			smt_yices_assert(yices_context, term, (*cnst)->obj);
	// 		}
	// 		cnst++;
	// 	}

	// 	//add mutated branch
	// 	smt_yices_assert(yices_context, (*it)->br->update_term(), (*it)->br);
		

	// 	call_to_solver++;
	// 	check_satisfiability();
	// 	if(solve_constraints(clock)){
	// 		selected_branch = (*it)->br;
	// 		selected_clock = clock;
	// 		//insert hash value even if potentially incorrect
	// 		update_path_taken((*it));
	// 		if(!enable_error_check){
	// 			(*it)->br->set_covered_clk(sim_num+1, clock);
	// 		}
	// 		// printf("[FOUND: CLOCK: %u, IDX: %u, DIST: %u, PROB: %f] %s", clock, selected_branch->list_idx, selected_branch->block->distance, selected_branch->branch_probability, it->br->print().c_str());
	// 		// printf("[FOUND: CLOCK: %u, IDX: %u, PROB: %f] %s", clock, selected_branch->list_idx, selected_branch->branch_probability, it->br->print().c_str());
			
	// 		// Xiangchen: Adjust the probability
	// 		if(g_is_new_block){
	// 			SMTBranch::increase_probability(selected_branch);
	// 		}else{
	// 			if(sim_num > prob_num){
	// 				SMTBranch::decrease_probability(selected_branch);
	// 			}else{
	// 				SMTBranch::increase_probability(selected_branch);
	// 			}
	// 		}
	// 		// SMTBranch::print_probability();
	// 		selected_branch->k_permit_covered++;
	// 		// Xiangchen: Because we use the probability to sort the branches
	// 		// So we need not to consider the distance
	// 		selected_branch->block->distance++;
	// 		return true;
	// 	}
	// }



static void check_satisfiability(){
    //reset context
    yices_reset_context(yices_context);

    //insert initial assertion to zero for registers
    SMTSigCore::yices_insert_reg_init(yices_context);

    //dump yices constraints
    dump_yices_constraints(constraints_stack.size()-1);

    //check status
    if(!check_yices_status()){
        smt_yices_dump_error();
        error("Simulation not satisfiable");
    }

	// Dump constraints
	if(true){
		// info("Dumping constraints");
		smt_yices_dump_error();
	}

}

static SMTPath* concolic_iteration(SMTPath *curr_path) {
	SMTPath *init_path = NULL;
	init_path = new SMTPath(*curr_path);
    SMTPath *next_path = NULL;

    // SMTPath* step_path = NULL;
	if(enable_error_check){
		//check satisfiability
		check_satisfiability();
		//check if selected branch is covered
		if(selected_branch){
			if(!selected_branch->is_covered_clk(selected_clock)){
				error("Selected branch is not covered by simulation");
			}
		}
	}

	explore_one_step(curr_path);

	if(find_next_cfg(curr_path, init_path->data.get_clk(), curr_path->data.get_clk())){
        simulate_build_stack();
		next_path = new SMTPath(g_data_step);
    }
	return next_path;
}

// Read targets from branch_id
uint select_target() {
	uint branch_id;
	FILE* f_branch_id = fopen("branch_id", "r");
	assert(f_branch_id);
	info("Reading Target id from file \"branch_id\"");
	while (fscanf(f_branch_id, "%d", &branch_id) == 1) {
		SMTAssign* assign = SMTAssign::get_assign(branch_id);
		assert(assign->assign_type == SMT_ASSIGN_BRANCH);
		SMTBasicBlock::add_target(assign->block);
		iter_count[assign->block] = 0;
	}
	fclose(f_branch_id);
	return SMTBasicBlock::target_num();
}



void step_coverage() {
	SMTPath *path = NULL;
	SMTSigCore::clear_all_versions();
	//edge realignment

	SMTBasicBlock::edge_realignment();
	SMTBasicBlock::update_all_distances();
	SMTProcess::make_circular();

	// generate random seed
	srand(time(NULL));
	
	//random simulations
	for(uint i=0; i<g_random_sim_num; i++){
		//generate random inputs
		g_data.generate(g_data_mem);
		simulate_build_stack();   
		//save path
		path = new SMTPath(g_data);
		SMTBasicBlock::update_all_closest_paths(path, constraints_stack);
	}

	// remove covered
	SMTBasicBlock::remove_covered_targets(path->data.get_clk());

	// uint start_iteration = sim_num;
	
	//For every branch, it will give every branch a probability randomly
	SMTBranch::random_probability();

	while(!SMTBasicBlock::target_list.empty()){
		sim_num = 0;
		// erase covered target
		SMTBasicBlock* target = SMTBasicBlock::target_list.front();
		if (target->assign_list[0]->is_covered()) {
			SMTBasicBlock::target_list.pop_front();
			SMTBasicBlock::covered_target_list.push_back(target);
			continue;
		}

		// update distance from adjacency list
		target->update_distance_from_adjacency_list();
		printf("\nTrying to cover %s", target->assign_list[0]->print().c_str());

		// select the closest path from random simulation
		if (target->closest_path && target->closest_path != path) {
			path = target->closest_path;
			path->Dump(g_data_mem);
			simulate_build_stack();
		}
		selected_branch = NULL;
	
		while((path = concolic_iteration(path))){
			SMTBasicBlock::print_cover_result();
			if(path->data.get_clk() > g_unroll){
				if(target->assign_list[0]->is_covered()){
					SMTBasicBlock::remove_covered_targets(path->data.get_clk());
					break;
				}
				path->ClearPath();
				constraints_stack.clear();
				sim_num ++;
				explore_one_step(path);
			}
			if(sim_num >= total_limit){
				SMTBasicBlock::target_list.pop_front();
				SMTBasicBlock::uncovered_target_list.push_back(target);
				break;
			}
			//check if target covered or iteration limit reached
			if(target->assign_list[0]->is_covered()){
				SMTBasicBlock::remove_covered_targets(path->data.get_clk());
				break;
			}
			// if(path){
			// 	SMTBasicBlock::update_all_closest_paths(path, constraints_stack);
			// }
		}


	}
}


// Using multi target
void multi_coverage() {
	SMTPath *path = NULL;
	//reset variable versions to zero
	SMTSigCore::clear_all_versions();
	
	//edge realignment
	SMTBasicBlock::edge_realignment();
	SMTBasicBlock::update_all_distances();
	SMTProcess::make_circular();
	
	// ofstream f_cfg("cfg");
	// SMTBasicBlock::print_all(f_cfg);
	// f_cfg.close();
	
	// generate random seed
	srand(time(NULL));
	//random simulations
	for(uint i=0; i<g_random_sim_num; i++){
		//generate random inputs
		g_data.generate(g_data_mem);
		simulate_build_stack();   
		//save path
		path = new SMTPath(g_data);
		SMTBasicBlock::update_all_closest_paths(path, constraints_stack);
	}

	// remove covered
	SMTBasicBlock::remove_covered_targets(sim_num);
	sim_num = g_random_sim_num;
	while(!SMTBasicBlock::target_list.empty()){
		// Initalize the probability of every branch
		//For every target, it will give every branch a probability randomly
		SMTBranch::random_probability();
		// For every target, it will give every branch a probability based on the distance
		// SMTBranch::distance_probability();

		SMTBasicBlock* target = SMTBasicBlock::target_list.front();
		if (target->assign_list[0]->is_covered() /*|| iter_count[target] >= total_limit*/) {
			SMTBasicBlock::target_list.pop_front();
			continue;
		}
		target->update_distance_from_adjacency_list();
		printf("\nTrying to cover %s", target->assign_list[0]->print().c_str());
		iter_count[target] += iteration_limit;
		if (target->closest_path && target->closest_path != path) {
			// select the closest path from fuzzing process
			path = target->closest_path;

			// printf("closest distance: %u\n", target->closest_path_distance);
			//update block distance from target's adjacency_list
			//Yangdi: Critical Error of first version
			//After selecting the path, signal clock versions need to be updated
			
			path->Dump(g_data_mem);
			simulate_build_stack();
		}
		//do iterations till not covered or iteration limit reached
		selected_branch = NULL;
		uint start_iteration = sim_num;

		while((path = concolic_iteration(path))){
			sim_num++;  
			//check if target covered or iteration limit reached
			if(target->assign_list[0]->is_covered() || (sim_num - start_iteration) > iteration_limit){
				//erase covered target
				SMTBasicBlock::remove_covered_targets(sim_num);
				break;
			}
		}
		if (/*target->assign_list[0]->is_covered() ||*/ iter_count[target] >= total_limit) {
			if(target->assign_list[0]->is_covered())
				continue;
			SMTBasicBlock::target_list.pop_front();
			continue;
		}

		if (path)
			SMTBasicBlock::update_all_closest_paths(path, constraints_stack);

		if (!target->assign_list[0]->is_covered()) {
			target->closest_path_distance+=iteration_limit / 5;
			//generate random inputs
			for (uint i = 0; i < 2; i++) {
				g_data.generate(g_data_mem);
				simulate_build_stack();   
				//save path
				path = new SMTPath(g_data);
				SMTBasicBlock::update_all_closest_paths(path, constraints_stack);
			}
			SMTBasicBlock::target_list.pop_front();
			SMTBasicBlock::target_list.push_back(target);
			SMTBasicBlock::print_cover_result();
			target->dump_distances();
		}
	}
}

void dump_branch_id() {
	if(file_exist("branch_id")){
		return ;
	}else{
		ofstream bisFile("branch_id",ios::out);
		uint assign_counts = SMTAssign::get_assign_count();
		for(uint i = 0;i < assign_counts; ++i){
			SMTAssign* assign = SMTAssign::get_assign(i);
			if(assign->assign_type==SMT_ASSIGN_BRANCH){
				bisFile << i <<endl;
			}
		}
		bisFile.close();
		exit(0);
	}
	return ;
}

void start_concolic() {
	sim_num = 0;
	init();
	// multi target to cover all
	dump_branch_id();
	uint target_count = select_target();
	if(target_count == 0){
		error("No target specified");
	} else{
		info("Total targets: %d", target_count);
	}
	SMTBranch::target_count = target_count;
	start_time = clock();

	// multi_coverage();
	step_coverage();
	end_concolic();
}


void end_concolic(){
    clock_t end_time = clock();
	fflush(stdout);
    
    // ofstream report("report_cov.log");
    // report << "[TIME] " << difftime(end_time, start_time) << " sec\n";
	// report << "[TIME] " << (end_time - start_time)/double(CLOCKS_PER_SEC) << " sec\n";
	// report << "[ITER] " << sim_num << '\n';
    // report << "[SOLVER CALL] " << call_to_solver << '\n';
	// report << "[CNST BEFORE] " << total_constraints_before << '\n';
	// report << "[CNST AFTER] " << total_constraints_after << '\n';
	// report << "[TOTAL BRANCH] " << SMTBranch::total_branch_count << '\n';
	// report << "[COVERED BRANCH] " << SMTBranch::covered_branch_count << '\n';
    // SMTAssign::print_coverage(report);
    // report.close();
    

	
	printf("\nCovered branch number: %ld, Uncovered branch number: %ld, Coverage rate: %.2f%%\n",  SMTBasicBlock::covered_target_list.size(), SMTBasicBlock::uncovered_target_list.size(), (double)SMTBasicBlock::covered_target_list.size() / (SMTBranch::target_count) * 100);

	printf("[TIME] %.2lf sec\n", (end_time - start_time)/double(CLOCKS_PER_SEC));



    yices_free_context(yices_context);
    yices_exit();
    SMTFreeAll();
    exit(0);
}
