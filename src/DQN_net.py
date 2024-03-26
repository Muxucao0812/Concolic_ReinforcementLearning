import torch
import re
import numpy as np
import random
import collections
import pickle
import os
import collections
import random
from torch.nn import functional as F

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
    state = convert_values_to_int(state_dict)
    state_array = np.array(list(state.values())) 
    np.savetxt('state_now.txt', state_array, fmt='%d')
    return state_array


def get_action(file_path):
    cycles_data = parse_sim_log(file_path)
    return int(cycles_data['Action'])

def get_reward(file_path):
    cycles_data = parse_sim_log(file_path)
    return float(cycles_data['reward'])

def shuffle_and_return(lst):
    # 复制列表以避免修改原始数据
    shuffled_list = lst.copy()
    random.shuffle(shuffled_list)
    # print(f"Random list:{shuffled_list}")
    return shuffled_list


    
class Qnet(torch.nn.Module):
    def __init__(self, state_dim, hidden_dim, action_dim):
        super(Qnet, self).__init__()
        self.fc1 = torch.nn.Linear(state_dim, 512)
        self.fc2 = torch.nn.Linear(512, 256)  
        self.fc3 = torch.nn.Linear(256, action_dim)

    
    def forward(self, x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))  
        x = F.relu(self.fc3(x))  
        return torch.sigmoid(x)  
    
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
            # return [random.choice(branch_list)]
            return shuffle_and_return(branch_list)
        with torch.no_grad():  # 在进行推断时不需要计算梯度
            action_probs = self.q_net(state_tensor)
            action_probs = action_probs.cpu().numpy()  # 转换为numpy数组，假设你的模型输出的是概率
        # 获取按概率排序的动作索引
        sorted_indices = np.argsort(action_probs[0])[::-1]
        # 根据排序后的索引获取对应的branch_list
        sorted_branch_list = [branch_list[i] for i in sorted_indices]
        # print(f"Q learn sorted_branch_list:{sorted_branch_list}")
        return sorted_branch_list


    def update(self, state, action, reward, next_state, done):
        # 转换为适合网络处理的格式
        state_tensor = torch.tensor([state], dtype=torch.float).to(self.device)
        action_tensor = torch.tensor([action], dtype=torch.int64).view(-1, 1).to(self.device)
        reward_tensor = torch.tensor([reward], dtype=torch.float).view(-1, 1).to(self.device)
        next_state_tensor = torch.tensor([next_state], dtype=torch.float).to(self.device)
        done_tensor = torch.tensor([done], dtype=torch.float).view(-1, 1).to(self.device)

        # 计算当前状态的Q值
        q_values = self.q_net(state_tensor).gather(1, action_tensor)
        # 计算下一个状态的最大Q值
        max_next_q_values = self.target_q_net(next_state_tensor).max(1)[0].view(-1, 1)
        # 计算目标Q值
        q_targets = reward_tensor + self.gamma * max_next_q_values * (1 - done_tensor)

        # 计算损失
        dqn_loss = torch.mean(F.mse_loss(q_values, q_targets))

        # 梯度下降更新网络参数
        self.optimizer.zero_grad()
        dqn_loss.backward()
        self.optimizer.step()

        # 更新目标网络
        if self.count % self.target_update == 0:
            self.target_q_net.load_state_dict(self.q_net.state_dict())
        self.count += 1