from DQN_net import *
from DQN_init import *
import warnings
import time
import json
# 忽略所有警告
warnings.filterwarnings("ignore")

# DQN参数
learning_rate = 0.1
gamma = 0.9
epsilon = 0.6
target_update = 5
capacity = 10000
batch_size = 4
episodes = 500   # 总迭代轮次
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
done = False

#初始化环境
state = np.loadtxt('state_now.txt', dtype=int)

# 环境参数
branch_dim = 348 #Need to change by different test
branch_list = range(0,branch_dim)
state_dim = len(state)  # 状态维度
action_dim = len(branch_list) # 动作维度
hidden_dim = 32  # 隐藏层维数



# 初始化DQN代理
dqn_agent = DQN(state_dim, hidden_dim, action_dim, learning_rate, gamma, epsilon, target_update, device)

# 检查模型参数文件是否存在
q_path = 'q_net_model_parameters.pth'
if os.path.isfile(q_path):
    dqn_agent.q_net.load_state_dict(torch.load(q_path))  # 加载模型参数
    # print("Loaded Q-network model parameters.")

target_q_path = 'target_q_net_model_parameters.pth'
if os.path.isfile(target_q_path):
    dqn_agent.target_q_net.load_state_dict(torch.load(target_q_path))  # 可能你想加载到target_q_net
    # print("Loaded target Q-network model parameters.")

action = get_action("data.state")
reward = get_reward("data.state")
next_state = get_state("data.state")
# next_state = get_state("model.log")



dqn_agent.update(state, action, reward, next_state, done)
# 保存网络的参数
torch.save(dqn_agent.q_net.state_dict(), 'q_net_model_parameters.pth')
torch.save(dqn_agent.target_q_net.state_dict(), 'target_q_net_model_parameters.pth')


