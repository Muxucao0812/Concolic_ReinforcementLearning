
import sys
import random
import numpy as np
import collections
import torch
import torch.nn.functional as F
sys.path.append('.')  # 添加当前目录到Python路径

# DQN参数
learning_rate = 0.001
gamma = 0.99
epsilon = 0.1
target_update = 10
capacity = 10000
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
done = False

#初始化环境
#env = RegistersEnv(state_int)

# 环境参数
branch_list = range(0,347)
state_int = get_state("data.state")
state_dim = len(state_int)  # 状态维度
action_dim = len(branch_list) # 动作维度
hidden_dim = 32  # 隐藏层维数

# 初始化DQN代理和回放缓冲区
dqn_agent = DQN(state_dim, hidden_dim, action_dim, learning_rate, gamma, epsilon, target_update, device)
replay_buffer = ReplayBuffer(capacity)