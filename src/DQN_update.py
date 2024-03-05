from DQN import *

action = get_action("data.state")
reward = get_reward("data.state")
next_state = get_state("data.state")
next_state = np.array(list(next_state.values()))  # 将下一个状态字典转换为数组
replay_buffer.add(state_int, action, reward, next_state, done)  # 保存到回放缓冲区
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