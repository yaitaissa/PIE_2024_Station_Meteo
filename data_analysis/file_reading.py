import numpy as np
import calendar


def add_line_to_dict(line, dict):
    date = line[0]
    values = []
    for i in range(1,len(line)):
       values.append(float(line[i]))
    
    dict[date] = values




def get_date_interval(date:str, type:str, spacing:int):
    local_date = date.split(' ')
    time = local_date[-1].split(':')

    month = list(calendar.month_name).index(local_date[0])
    day = int(local_date[1])
    year = int(local_date[2])
    hour = int(time[0])
    minutes = int(time[1])
    seconds = int(time[2])

    if type == 'year':
        month, day, hour, minutes, seconds = calendar.month_name[1],1,0,0,0
        year = year - year%spacing

    elif type == 'month':
        day, hour, minutes, seconds = 1,0,0,0
        month =  month - month%spacing
    
    elif type == 'day':
        hour, minutes, seconds = 0,0,0
        day = day - day%spacing

    elif type == 'hour':
        minutes, seconds = 0,0
        hour = hour - hour%spacing
    
    elif type == 'minutes':
        seconds = 0
        minutes = minutes - minutes%spacing
    
    elif type == 'seconds':
        seconds = seconds - seconds%spacing
    
    return f'{calendar.month_name[month]} {day} {year} {hour}:{minutes}:{seconds}'



def sort_by_interval(dict, type, spacing):
    new_dict = {}
    dates = list(dict.keys())

    for date in dates:
        new_date = get_date_interval(date, type, spacing)

        if new_date not in new_dict:
            new_dict[new_date] = []

        new_dict[new_date].append(dict[date])
    
    for new_date in new_dict:
        new_dict[new_date] = np.array(new_dict[new_date]).mean(axis=0)

    return new_dict

def fuze_dict(list_dict:list[dict], nb_values:int, nb_dict:int):
    list_values = []
    
    for i in range(nb_values):
        dict_values = {}

        for j in range(nb_dict):
            dict = list_dict[j]

            for date in dict:
                if date not in dict_values:
                    dict_values[date] = {}
                
                dict_values[date][j] = dict[date][i]

        list_values.append(dict_values)
    
    return list_values


file = open('sensor_data_1.txt')
lines = file.readlines()
file.close()

datas_1 = {}
for i in range(1,len(lines)):
    line = lines[i]
    line = line.rstrip('\n').split(',')
    add_line_to_dict(line, datas_1)

print("Extrating values 1\n")
print(datas_1)
print("\n\nRegrouping values 1\n")
datas_1 = sort_by_interval(datas_1, 'minutes', 1)
print(datas_1)


file = open('sensor_data_2.txt')
lines = file.readlines()
file.close()

datas_2 = {}
for i in range(1,len(lines)):
    line = lines[i]
    line = line.rstrip('\n').split(',')
    add_line_to_dict(line, datas_2)

print("Extrating values 2\n")
print(datas_2)
print("\n\nRegrouping values 2\n")
datas_2 = sort_by_interval(datas_2, 'minutes', 1)
print(datas_2)



print("\n\nFuzing values\n")

values = fuze_dict([datas_1,datas_2], 5, 2)
print(values[0])
print(len(values))
print(len(values[0]))