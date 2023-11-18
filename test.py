#! /usr/bin/env python
import subprocess
from time import sleep
import time

cmd = ["./build/build_my_own_sqlite", "./mydb.db"]

test_input = []

for i in range(100):
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
for i in range(101):
    input_texts =test_input[i]
    time.sleep(1)
    # 发送输入数据并获取输出

    for input_text in input_texts:
        proc.stdin.write(input_text)
        proc.stdin.flush()
        
