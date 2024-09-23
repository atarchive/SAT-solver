def read_cnf(filename):
    """读取 CNF 文件并返回变量数、子句列表"""
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    clauses = []
    num_vars = 0
    
    for line in lines:
        if line.startswith('p cnf'):
            parts = line.split()
            num_vars = int(parts[2])
        elif line.startswith('%'):
            continue
        elif line.startswith('0'):
            continue
        elif line.startswith('c') or len(line.strip()) == 0:
            continue
        else:
            clause = list(map(int, line.split()))[:-1]  # 去掉最后的0
            clauses.append(clause)

    return num_vars, clauses

def evaluate_clause(clause, assignment):
    """根据赋值判断子句的值"""
    for literal in clause:
        var_index = abs(literal) - 1
        if (literal > 0 and assignment[var_index]) or (literal < 0 and not assignment[var_index]):
            return True
    return False

def check_assignment(cnf_file, assignment):
    """检查赋值是否满足 CNF"""
    num_vars, clauses = read_cnf(cnf_file)

    print(f"变量数量: {num_vars}, 赋值长度: {len(assignment)}")
    
    if len(assignment) != num_vars:
        raise ValueError(f"赋值长度必须为 {num_vars}，但提供的赋值长度为 {len(assignment)}。")

    for clause in clauses:
        #print(f"当前检查的子句: {clause}")
        if not evaluate_clause(clause, assignment):
            print(f"不满足的子句: {clause}")  # 添加调试输出
            return False  # 找到一个不满足的子句

    return True  # 所有子句都满足
def read_assignment(filename):
    """从文件中读取赋值并返回列表"""
    with open(filename, 'r') as f:
        line = f.readline()  # 读取第一行
        assignment = list(map(int, line.strip().split()))  # 将字符串分割并转换为整数
    return assignment
cnt=0
for i in range(101,301):
    # 示例用法
    assignment_file = "E:\\answers\\answer"+str(i)+".txt"  # 文件名
    assignment = read_assignment(assignment_file)
    # 在这里定义 CNF 文件名和赋值
    cnf_file= "E:\\cnfs\\uf20-0"+str(i)+".cnf"  # 请根据实际路径调整
    # 将赋值从0/1转换为True/False
    assignment_bool = [bool(x) for x in assignment]

    result = check_assignment(cnf_file, assignment_bool)

    if result:
        cnt+=1
print(cnt)
