numbers = input("输入样例：\r\n").split()

print("输出样例：", end="\r\n")
for i in range(len(numbers)):
    num = int(numbers[i], 16)
    print(f"0x{num:02X}", end="")
    if i != len(numbers) - 1:
        print(",", end=" ")
print()
