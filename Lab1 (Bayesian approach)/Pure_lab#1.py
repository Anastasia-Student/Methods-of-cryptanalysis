import pandas as pd
import numpy as np
Prob_df_09 = pd.read_csv('prob_09.csv', header=None)
Table_df_09 = pd.read_csv('table_09.csv', header=None)
# Hurray! Datasets #9 are alredy stored in a Pandas Dataframe
Prob_df_09 = Prob_df_09.rename(index={0 : 'P(M_i)', 1: 'P(K_i)'})
Prob_array_09 = np.array(Prob_df_09)
Table_array_09 = np.array(Table_df_09)
# Searching for unique values C_h and their amount
(unique_values, amount) = np.unique(Table_array_09, return_counts=True)
frequencies = np.asarray((unique_values, amount))
print(frequencies)
prob_C_h_vector =[0]*frequencies.shape[1]

for C_h in range(frequencies.shape[1]):
  i = 0
  for table_ind_i in range(Table_array_09.shape[0]):
    for table_ind_j in range(Table_array_09.shape[1]):
      if (C_h == Table_array_09[table_ind_i][table_ind_j]):
        #print('i = ', i)
        #print('C_h = ', C_h)
        #print('Prob_array_09[0][',table_ind_i,'] = ', Prob_array_09[0][table_ind_i])
        #print('Prob_array_09[1][',table_ind_j,'] = ', Prob_array_09[1][table_ind_j])
        i += 1
        prob_C_h_vector[C_h] += Prob_array_09[0][table_ind_i] * Prob_array_09[1][table_ind_j]
      if (i == frequencies[1][C_h]):
        break
#print('prob_C_h_vector = ', prob_C_h_vector)
prob_C_h_vector_df = pd.DataFrame(prob_C_h_vector, index=['C_0','C_1','C_2', 'C_3','C_4','C_5','C_6','C_7','C_8', 'C_9','C_10','C_11','C_12','C_13','C_14','C_15','C_16','C_17','C_18', 'C_19']).T
prob_M_i_C_h_vector = [[0]*frequencies.shape[1] for i in range(frequencies.shape[1])]
for M_i in range(Prob_array_09.shape[1]):
  for C_h in range(frequencies.shape[1]):
    for K_j in range(Prob_array_09.shape[1]):
      if(C_h == Table_array_09[K_j][M_i]):
        prob_M_i_C_h_vector[C_h][M_i] += Prob_array_09[0][M_i] * Prob_array_09[1][K_j]

prob_M_i_C_h_vector = np.asarray(prob_M_i_C_h_vector).T
prob_M_i_C_h_vector_df = pd.DataFrame(prob_M_i_C_h_vector, columns=['C_0','C_1','C_2', 'C_3','C_4','C_5','C_6','C_7','C_8', 'C_9','C_10','C_11','C_12','C_13','C_14','C_15','C_16','C_17','C_18', 'C_19']).T
prob_M_i_C_h_vector_df = prob_M_i_C_h_vector_df.rename(columns={0 : 'M_0', 1: 'M_1', 2: 'M_2', 3: 'M_3', 4: 'M_4', 5:'M_5', 6:'M_6', 7: 'M_7', 8: 'M_8', 9 :'M_9', 10 : 'M_10', 11: 'M_11',12: 'M_12', 13: 'M_13', 14: 'M_14', 15: 'M_15',16: 'M_16',17 : 'M_17',18: 'M_18', 19: 'M_19'})
prob_C_h_vector = np.asarray(prob_C_h_vector).T
prob_M_i_C_h_vector = np.asarray(prob_M_i_C_h_vector).T
prob_M_i_cond_C_h_vector = [[0]*frequencies.shape[1] for i in range(frequencies.shape[1])]

for M_i_in_prob_M_i_C_h_vector in range(prob_M_i_C_h_vector.shape[1]):
  for C_h_in_prob_M_i_C_h_vector in range(prob_M_i_C_h_vector.shape[0]):
    prob_M_i_cond_C_h_vector[C_h_in_prob_M_i_C_h_vector][M_i_in_prob_M_i_C_h_vector] = prob_M_i_C_h_vector[C_h_in_prob_M_i_C_h_vector][M_i_in_prob_M_i_C_h_vector] / prob_C_h_vector[C_h_in_prob_M_i_C_h_vector]

prob_M_i_cond_C_h_vector_df = pd.DataFrame(prob_M_i_cond_C_h_vector, columns=['M_0','M_1','M_2', 'M_3','M_4','M_5','M_6','M_7','M_8', 'M_9','M_10','M_11','M_12','M_13','M_14','M_15','M_16','M_17','M_18', 'M_19'])
prob_M_i_cond_C_h_vector_df = prob_M_i_cond_C_h_vector_df.rename(index={0 : 'C_0', 1: 'C_1', 2: 'C_2', 3: 'C_3', 4: 'C_4', 5:'C_5', 6:'C_6', 7: 'C_7', 8: 'C_8', 9 :'C_9', 10 : 'C_10', 11: 'C_11',12: 'C_12', 13: 'C_13', 14: 'C_14', 15: 'C_15',16: 'C_16',17 : 'C_17',18: 'C_18', 19: 'C_19'})
prob_M_i_cond_C_h_vector = np.asarray(prob_M_i_cond_C_h_vector)
delta_eq_M = [0]*prob_C_h_vector.shape[0]
max_start_value_array = [0]*prob_M_i_cond_C_h_vector.shape[0]

for C_h in range(Table_array_09.shape[0]):
  for i in range(prob_M_i_cond_C_h_vector.shape[1]):
    if(max_start_value_array[C_h] <= prob_M_i_cond_C_h_vector[C_h][i]):
      max_start_value_array[C_h] = prob_M_i_cond_C_h_vector[C_h][i]
      delta_eq_M[C_h] = i

delta_eq_M_0 = np.asarray(delta_eq_M)
delta_eq_M_df = pd.DataFrame(delta_eq_M_0).T.rename(columns={0 : 'C_0', 1: 'C_1', 2: 'C_2', 3: 'C_3', 4: 'C_4', 5:'C_5', 6:'C_6', 7: 'C_7', 8: 'C_8', 9 :'C_9', 10 : 'C_10', 11: 'C_11',12: 'C_12', 13: 'C_13', 14: 'C_14', 15: 'C_15',16: 'C_16',17 : 'C_17',18: 'C_18', 19: 'C_19'}, index={0:'d'})
s_func_value_matrix = [[1/Table_df_09.shape[1]]*prob_C_h_vector.shape[0] for i in range(Table_df_09.shape[1])]
s_func_value_matrix = np.asarray(s_func_value_matrix).T
s_func_value_matrix_df = pd.DataFrame(s_func_value_matrix, columns=['M_0','M_1','M_2', 'M_3','M_4','M_5','M_6','M_7','M_8', 'M_9','M_10','M_11','M_12','M_13','M_14','M_15','M_16','M_17','M_18', 'M_19'])
s_func_value_matrix_df = s_func_value_matrix_df.rename(index={0 : 'C_0', 1: 'C_1', 2: 'C_2', 3: 'C_3', 4: 'C_4', 5:'C_5', 6:'C_6', 7: 'C_7', 8: 'C_8', 9 :'C_9', 10 : 'C_10', 11: 'C_11',12: 'C_12', 13: 'C_13', 14: 'C_14', 15: 'C_15',16: 'C_16',17 : 'C_17',18: 'C_18', 19: 'C_19'})
s_func_value_answer = [[-11111]*prob_C_h_vector.shape[0] for i in range(Table_df_09.shape[1])]
max_start_value_array_for_s_func = [0]*prob_M_i_cond_C_h_vector.shape[0]

for C in range(prob_M_i_cond_C_h_vector.shape[0]):
  for M in range(prob_M_i_cond_C_h_vector.shape[1]):
    if (delta_eq_M[C] == M):
      max_start_value_array_for_s_func[C] = prob_M_i_cond_C_h_vector[C][M]

max_start_value_array_for_s_func_df = pd.DataFrame(max_start_value_array_for_s_func, columns = ['max_prob'], index=['P(M|C_0)','P(M|C_1)','P(M|C_2)', 'P(M|C_3)','P(M|C_4)','P(M|C_5)','P(M|C_6)','P(M|C_7)','P(M|C_8)', 'P(M|C_9)','P(M|C_10)','P(M|C_11)','P(M|C_12)','P(M|C_13)','P(M|C_14)','P(M|C_15)','P(M|C_16)','P(M|C_17)','P(M|C_18)', 'P(M|C_19)]']).T
amount_of_M_i = [0]*prob_C_h_vector.shape[0]
for C_h in range(Table_array_09.shape[0]):
  for i in range(prob_M_i_cond_C_h_vector.shape[1]):
    if (s_func_value_matrix[C_h][i] > 0 and max_start_value_array_for_s_func[C_h] <= prob_M_i_cond_C_h_vector[C_h][i]):
      max_start_value_array_for_s_func[C_h] = prob_M_i_cond_C_h_vector[C_h][i]
      s_func_value_answer[i][C_h] = i
      amount_of_M_i[C_h] += 1

s_func_value_answer = np.asarray(s_func_value_answer).T
s_func_value_answer_df = pd.DataFrame(s_func_value_answer, columns=['M_0','M_1','M_2', 'M_3','M_4','M_5','M_6','M_7','M_8', 'M_9','M_10','M_11','M_12','M_13','M_14','M_15','M_16','M_17','M_18', 'M_19'])
s_func_value_answer_df = s_func_value_answer_df.rename(index={0 : 'C_0', 1: 'C_1', 2: 'C_2', 3: 'C_3', 4: 'C_4', 5:'C_5', 6:'C_6', 7: 'C_7', 8: 'C_8', 9 :'C_9', 10 : 'C_10', 11: 'C_11',12: 'C_12', 13: 'C_13', 14: 'C_14', 15: 'C_15',16: 'C_16',17 : 'C_17',18: 'C_18', 19: 'C_19'})
for C_i in range(s_func_value_matrix.shape[0]):
  for M_j in range(s_func_value_matrix.shape[1]):
    if(s_func_value_answer[C_i][M_j] == -11111):
      s_func_value_matrix[C_i][M_j] = 0
    else:
      s_func_value_matrix[C_i][M_j] = 1/amount_of_M_i[C_i]

s_func_value_matrix_df = pd.DataFrame(s_func_value_matrix, columns=['M_0','M_1','M_2', 'M_3','M_4','M_5','M_6','M_7','M_8', 'M_9','M_10','M_11','M_12','M_13','M_14','M_15','M_16','M_17','M_18', 'M_19'])
s_func_value_matrix_df = s_func_value_matrix_df.rename(index={0 : 'C_0', 1: 'C_1', 2: 'C_2', 3: 'C_3', 4: 'C_4', 5:'C_5', 6:'C_6', 7: 'C_7', 8: 'C_8', 9 :'C_9', 10 : 'C_10', 11: 'C_11',12: 'C_12', 13: 'C_13', 14: 'C_14', 15: 'C_15',16: 'C_16',17 : 'C_17',18: 'C_18', 19: 'C_19'})


from decimal import Decimal
av_loss_of_det_func = Decimal('0')
for M_i in range(prob_M_i_C_h_vector.shape[1]):
  for C_j in range(prob_M_i_C_h_vector.shape[0]):
    if (delta_eq_M_0[C_j] != s_func_value_answer[C_j][M_i]):
      av_loss_of_det_func += Decimal(prob_M_i_C_h_vector[C_j][M_i])

print("Average loss of deterministic function = ", av_loss_of_det_func)

av_loss_of_stoch_func = Decimal('0')
HELP_loss_of_stoch_func = [[Decimal('0')]*prob_M_i_C_h_vector.shape[1] for i in range(prob_M_i_C_h_vector.shape[0])]
for M_i in range(prob_M_i_C_h_vector.shape[1]):
  for M_j_not_M_i in range(prob_M_i_C_h_vector.shape[1]):
    for C_h in range(prob_M_i_C_h_vector.shape[0]):
      if (M_j_not_M_i != M_i):
        HELP_loss_of_stoch_func[C_h][M_i] += Decimal(s_func_value_matrix[C_h][M_j_not_M_i])

for C_j in range(prob_M_i_C_h_vector.shape[0]):
  for M_i in range(prob_M_i_C_h_vector.shape[1]):
    av_loss_of_stoch_func += Decimal(Decimal(prob_M_i_C_h_vector[C_j][M_i]) * Decimal(HELP_loss_of_stoch_func[C_j][M_i]))

print("Average loss of stochastic function = ", av_loss_of_stoch_func)
