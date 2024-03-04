#include "ivl_target.h"
#include "smt_lib.h"
#include "data_mem.h"
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

using namespace std;

//manualy select
bool            user_select_branch = false;


//global variables
FILE*			g_out = NULL;
FILE*			g_prob = NULL;
FILE*			g_cover_result = NULL;
const char*	    g_prob_file = "prob.txt";
const char*	    g_cover_result_file = "cover_result.txt";
int				g_errors = 0;
unsigned		g_ind = 0;
unsigned		g_ind_incr = 4;
ivl_design_t	g_design = 0;
const char*     g_output_file = "conquest_dut.v";
const char*     g_tb_file = "conquest_tb.v";
const char*		g_data_mem = "data.mem";
const char*		g_data_mem_step = "data_step.mem";
const char*		g_data_state = "data.state";
CTDataMem		g_data;
CTDataMem		g_data_step;
clock_t         start_time;
bool            g_is_new_block = false;       
bool            g_distance_decrease = false;
//Parameters
uint			g_unroll;
uint			g_step;
uint            g_sim_clk;
const char*     g_clock_sig_name;
const char*     g_reset_sig_name;
const char*     g_reset_edge_active;
uint            g_random_sim_num;
uint            prob_num = 50;

/*! \brief The name of experiment result file*/
const char*      g_name_exp_res = "exp_res.txt";

/*! \brief Global experiment result file*/
FILE*            g_exp_res;

//Qlearn parameters
double epsilon_qlearn = 0.6; // 探索率
double alpha_qlearn = 0.5; // 学习率
double gamma_qlearn = 0.8; // 折扣因子
std::unordered_map<std::string, std::vector<double>> q_table; //q_table


const char* dqn_init = R"(

import sys
import random
import gym
import numpy as np
import collections
from tqdm import tqdm
import torch
import torch.nn.functional as F
sys.path.append('.')  # 添加当前目录到Python路径

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

)";
const char* dqn_sort_branch = R"(
def out_sort_branch(state_path):
    state = get_state(state_path)
    state = np.array(list(state_int.values()))  # 将状态字典转换为数组
    sorted_branch_list = dqn_agent.take_action(state, list(range(0,347)))
    with open('sorted_branch_list.txt', 'w') as file:
        file.write(f"{sorted_branch_list}\n") 
out_sort_branch("data.state")  
)";
const char* dqn_update = R"(
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
)";