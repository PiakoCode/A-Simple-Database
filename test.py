#! /usr/bin/env python
import subprocess
from time import sleep
import time

cmd = ["./build/build_my_own_sqlite", "./mydb.db"]

test_input = []

epoch = 100

for i in range(epoch):
    test_input.append(f"insert {i} user{i} user{i}@mail.com\n")


test_input.append(".exit\n")
# 启动子进程
proc = subprocess.Popen(
    cmd,
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True,
)
for i in range(epoch + 1):
    
    input_texts =test_input[i]
    time.sleep(0.1)

    for input_text in input_texts:
        proc.stdin.write(input_text)
        proc.stdin.flush()
    print(f"epoch {i} finished!")
        
