import subprocess
import os

trace_weights = {
    "fail1.txt": 4,
    "fail2.txt": 4,
    "short1-bal.txt": 4,
    "short2-bal.txt": 4,
    "coalesce.txt": 4,
    "amptjp-bal.txt": 8,
    "binary-bal.txt": 8,
    "cccp-bal.txt": 8,
    "coalescing-bal.txt": 8,
    "cp-decl-bal.txt": 8,
    "random-bal.txt": 10,
}


try:
   os.makedirs("outputs/")
except FileExistsError:
   # directory already exists
   pass


results = []
total_grade = 0
total_weights = 0

results.append("|{:<20}|{:^50} |{:^17}".format(
        "", 
        "Correctness",
        "Score",
        ))
results.append("|{:<20}|{:^13}|{:^13}|{:^13}| {:^7} | {:^6} | {:^6}".format(
        "Trace", 
        "Success",
        "Offset",
        "Hops",
        "Avg",
        "Weight",
        "Score",
        ))
results.append("|--------------------|---------------------------------------------------|----------------")

for trace in trace_weights:
    output_path = "outputs/" + trace.replace("txt", "out")
    expected_output_path = "expected_outputs/" + trace.replace("txt", "out")
    subprocess.run(["./test", "-t", "traces/"+trace, "-o", output_path]) 
    lines = []
    expected_lines = []
    with open(output_path, 'r') as f:
        lines = f.readlines()
    with open(expected_output_path, 'r') as f:
        expected_lines = f.readlines()
    num_success_correct = 0
    num_offset_correct = 0
    num_hops_correct = 0

    for i in range(len(expected_lines)):
        if i >= len(lines):
            break

        line = lines[i]
        status = line.strip().split('\t')
        success = int(status[0])
        offset = int(status[1])
        hops = int(status[2])

        expected_line = expected_lines[i]
        expected_status = expected_line.strip().split('\t')
        expected_success = int(expected_status[0])
        expected_offset = int(expected_status[1])
        expected_hops = int(expected_status[2])

        if success == expected_success:
            num_success_correct += 1
        if offset == expected_offset:
            num_offset_correct += 1
        if hops == expected_hops:
            num_hops_correct += 1
    
    correctness = (num_success_correct + num_offset_correct + num_hops_correct) / (len(expected_lines) * 3)
    weight = trace_weights[trace]
    grade = correctness * weight
    total_grade += grade
    total_weights += weight
    results.append("|{:<20}|{:>6}/{:<6}|{:>6}/{:<6}|{:>6}/{:<6}| {:>6}% | {:>6} | {:>6}".format(
        trace, 
        num_success_correct, len(expected_lines),
        num_offset_correct, len(expected_lines),
        num_hops_correct, len(expected_lines),
        "{:.2f}".format(correctness*100),
        weight,
        "{:.2f}".format(grade),
        ))

results.append("|--------------------|---------------------------------------------------|----------------")

results.append("|{:<20}|{:^50} | {:>6}   {:>6}".format(
        "Total", 
        "",
        "",
        "{:.2f}".format(total_grade),
        ))
        
for result in results:
    print(result)
print()
print("Your grade: {}/{}".format("{:.2f}".format(total_grade), total_weights))

with open("00-grade-malloc.txt", 'w') as f:
    f.write("malloc project grade: {}/{}\n".format("{:.2f}".format(total_grade), total_weights))
    f.write("\nDetails:\n")
    for result in results:
        f.write(result)
        f.write("\n")


                