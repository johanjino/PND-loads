data = {50: 0.058247918821097075, 30: 0.05699888694202614, 75: 0.05975530880866768, 65: 0.061875303873922274, 57: 0.05146983503885119, 54: 0.061212137689260426, 52: 0.06390465572664071, 51: 0.06609412062802256}

def normalize_and_print(data):
    max_val = max(data.values())
    for key in sorted(data.keys()):
        norm_val = data[key] / max_val if max_val != 0 else 0
        print(f"({key}, {norm_val:.4f})")

normalize_and_print(data)
