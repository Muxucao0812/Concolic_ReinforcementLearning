import numpy as np
import random
import torch
import torch.nn.functional as F
import sys
from DQN import *

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

def update_state(state_new_path):
    action = get_action(state_new_path)
    reward = get_reward(state_new_path)
    next_state = get_state(state_new_path)
    next_state = np.array(list(next_state.values()))  # 将下一个状态字典转换为数组
    replay_buffer.add(state, action, reward, next_state, done)  # 保存到回放缓冲区
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


update_state("data.state")