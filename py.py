def sum(x, y):
    x = int(x)
    y = int(y)
    return x + y

def function2(x):
    return int(x)

def function3(x):
    return x

def function4(x):
    for x in range(1, x, 1):
        x = x + 1
    return x

def function5(x):
    return f'O numero é {x}.'

def function6(x):
    "I count until x. x is the number received."
    list = []
    for x in range(1, x, 1):
        list.append(x)
    return list

def function7(a, b, c):
    for i in range(1, 100000, 1):
        sum = a + b + c + i
    return sum

def thread_test():
    import time
    # sleep for 4 seconds
    time.sleep(5.2)
    # make hour of the day
    hour = time.strftime("%H")
    # make minute of the hour
    minute = time.strftime("%M")
    # make second of the minute
    second = time.strftime("%S")
    return f'{hour}:{minute}:{second}'
