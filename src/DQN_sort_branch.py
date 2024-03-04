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

def out_sort_branch(state_path):
    state = get_state(state_path)
    state = np.array(list(state_int.values()))  # 将状态字典转换为数组
    sorted_branch_list = dqn_agent.take_action(state, list(range(0,347)))
    with open('sorted_branch_list.txt', 'w') as file:
        file.write(f"{sorted_branch_list}\n") 
out_sort_branch("data.state") 