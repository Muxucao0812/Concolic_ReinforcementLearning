from DQN_net import *
from DQN_init import *
import warnings
import time
import json
# 忽略所有警告
warnings.filterwarnings("ignore")

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
state = get_state("data.state")
state = np.array(list(state.values()))

# 环境参数
branch_dim = 348 #Need to change by different test
branch_list = range(0,branch_dim)
state_dim = len(state)  # 状态维度
action_dim = len(branch_list) # 动作维度
hidden_dim = 32  # 隐藏层维数


# 记录开始时间
start_io_time = time.time()
# 初始化DQN代理
dqn_agent = DQN(state_dim, hidden_dim, action_dim, learning_rate, gamma, epsilon, target_update, device)

# 检查模型参数文件是否存在
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
end_io_time = time.time()
# 计算运行时间
io_time = end_io_time - start_io_time
print(f"update读参数文件时间：{io_time}秒")



# 记录开始时间
start_time = time.time()
action = get_action("data.state")
reward = get_reward("data.state")
next_state = get_state("data.state")
next_state = np.array(list(next_state.values()))  # 将下一个状态字典转换为数组

replay_buffer.add(state, action, reward, next_state, done)  # 保存到回放缓冲区
# 检查回放缓冲区是否足够大以开始学习
if replay_buffer.size() > batch_size:
    transitions = replay_buffer.sample(batch_size)  # 从回放缓冲区采样
    dqn_agent.update({
        'states': np.vstack(transitions[0]).astype(np.float32),
        'actions': transitions[1],
        'rewards': transitions[2],
        'next_states': np.vstack(transitions[3]).astype(np.float32),
        'dones': transitions[4]
    })
# 保存Q网络的参数
torch.save(dqn_agent.q_net.state_dict(), 'q_net_model_parameters.pth')

# 保存目标Q网络的参数
torch.save(dqn_agent.target_q_net.state_dict(), 'target_q_net_model_parameters.pth')

# 保存replay_buffer
with open('replay_buffer.pkl', 'wb') as f:
    pickle.dump(replay_buffer, f)
# 记录结束时间
end_time = time.time()

# 计算运行时间
elapsed_time = end_time - start_time
print(f"update程序运行时间：{elapsed_time}秒")



# # 获取模型参数的字典
# state_dict_q_net = dqn_agent.q_net.state_dict()
# # 转换参数字典为可以保存为JSON的格式
# state_dict_serializable_q_net = {k: v.tolist() for k, v in state_dict_q_net.items()}
# # 文件路径
# file_path_q_net = f'q_net.txt'
# # 保存参数字典到文件
# with open(file_path_q_net, 'w') as f:
#     json.dump(state_dict_serializable_q_net, f, indent=4)
    
# # 获取模型参数的字典
# state_dict_target_q_net = dqn_agent.target_q_net.state_dict()
# # 转换参数字典为可以保存为JSON的格式
# state_dict_serializable_target_q_net = {k: v.tolist() for k, v in state_dict_target_q_net.items()}
# # 文件路径
# file_path_target_q_net = f'target_q_net.txt'
# # 保存参数字典到文件
# with open(file_path_target_q_net, 'w') as f:
#     json.dump(state_dict_serializable_target_q_net, f, indent=4)