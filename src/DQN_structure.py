
import sys
import random
import numpy as np

import collections
import torch
import torch.nn.functional as F
from DQN import *
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