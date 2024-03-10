
from DQN_net import *
from DQN_init import * 
import warnings
import time



# 忽略所有警告
warnings.filterwarnings("ignore")


# DQN参数
learning_rate = 0.01
gamma = 0.99
epsilon = 0.4
target_update = 10 
capacity = 10000
batch_size = 16
episodes = 500   # 总迭代轮次
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
done = False

#初始化环境
state = get_state("data.state")
state = np.array(list(state.values()))

# 环境参数
branch_dim = 348 #Need to change by different test
branch_list = range(0,branch_dim)
state_dim = len(state)  # 状态维度
action_dim = len(branch_list) # 动作维度
hidden_dim = 32  # 隐藏层维数


# 初始化DQN代理
dqn_agent = DQN(state_dim, hidden_dim, action_dim, learning_rate, gamma, epsilon, target_update, device)
# # 将列表转换回torch.Tensor
# q_net_parameter_tensors = {k: torch.tensor(v) for k, v in q_net_parameter.items()}
# target_q_net_parameter_tensors = {k: torch.tensor(v) for k, v in target_q_net_parameter.items()}

# dqn_agent.q_net.load_state_dict(q_net_parameter_tensors)
# dqn_agent.target_q_net.load_state_dict(target_q_net_parameter_tensors)

#检查模型参数文件是否存在
q_path = 'q_net_model_parameters.pth'
if os.path.isfile(q_path):
    dqn_agent.q_net.load_state_dict(torch.load(q_path))  # 加载模型参数
    print("Loaded Q-network model parameters.")

target_q_path = 'target_q_net_model_parameters.pth'
if os.path.isfile(target_q_path):
    dqn_agent.target_q_net.load_state_dict(torch.load(target_q_path))  # 可能你想加载到target_q_net
    print("Loaded target Q-network model parameters.")

# 初始化回放缓冲区
replay_buffer = ReplayBuffer(capacity)

# 检查replay_buffer文件是否存在
replay_buffer_path = 'replay_buffer.pkl'
if os.path.isfile(replay_buffer_path):
    with open(replay_buffer_path, 'rb') as f:
        replay_buffer = pickle.load(f)  # 加载replay_buffer
        print("Loaded replay buffer.")




# 记录开始时间
state = get_state("data.state")
state = np.array(list(state.values()))  # 将状态字典转换为数组
sorted_branch_list = dqn_agent.take_action(state, list(range(0,branch_dim)))
with open('sorted_branch_list.txt', 'w') as file:
    file.write(f"{sorted_branch_list}\n") 

