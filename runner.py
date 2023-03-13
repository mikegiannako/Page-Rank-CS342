import subprocess
import time
import statistics

NUM_RUNS : int = 5
NUM_THREADS : int = 4

test_folder : str = "tests"

test_sets : list[str] = [
    "Email-Enron.txt",
    "facebook_combined.txt",
    "p2p-Gnutella24.txt"
]

args_sets : list[list] = [[test_folder + "/" + test, str(thread_count), "-m", "1"] for test in test_sets for thread_count in range(1, NUM_THREADS + 1)]

# Checks if the executable exists, else runs make
try:
    subprocess.run(["./page_rank.exe"], capture_output=True) # Even if the executable exists, it will just invoke a message 
                                                             # saying that the user needs to provide arguments
except FileNotFoundError:
    subprocess.run(["make"])

results : dict[str, dict[str, dict[str, float]]] = {}
for args in args_sets:
    times = []
    for i in range(NUM_RUNS):
        start_time = time.time()
        subprocess.run(["./page_rank.exe", *args], capture_output=True)
        end_time = time.time()
        times.append(end_time - start_time)
    avg_time = sum(times) / len(times)
    std_dev = statistics.stdev(times)
    
    # Calculate average speedup
    if args[1] == "1": avg_speedup = 1
    else:
        single_thread_time = results[f"{args[0]}"]["threads_1"]['avg_time']
        avg_speedup = single_thread_time / avg_time
    
    if f"{args[0]}" not in results:
        results[f"{args[0]}"] = {}

    results[f"{args[0]}"][f"threads_{args[1]}"]= {'avg_time': avg_time, 'std_dev': std_dev, 'avg_speedup': avg_speedup}

# before printing and saving the results, remove the prefix and suffix from the test names
for test in results:
    test_name = test.split("/")[-1]
    test_name = test_name.split(".")[0]
    results[test_name] = results.pop(test)

for res in results:
    print(f"{res}: {results[res]}")

# Save results to json file
import json
with open('results.json', 'w') as f:
    json.dump(results, f)
