import numpy as np
import random
import torch
import torch.nn.functional as F
import sys
from DQN import *


def out_sort_branch(state_path):
    state = get_state(state_path)
    state = np.array(list(state_int.values()))  # 将状态字典转换为数组
    sorted_branch_list = dqn_agent.take_action(state, list(range(0,347)))
    with open('sorted_branch_list.txt', 'w') as file:
        file.write(f"{sorted_branch_list}\n") 
        
out_sort_branch("data.state") 