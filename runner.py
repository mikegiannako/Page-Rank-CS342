import subprocess
import time
import statistics
import argparse

# Example execution 
# python my_script.py --num_runs 10 --num_threads 8 --exec_name my_executable --test_folder_path my_tests --test_sets test1.txt test2.txt

def arg_parser():
    # Define the argument parser
    parser = argparse.ArgumentParser(description='Run Page Rank tests')

    # Add the arguments
    parser.add_argument('--num_runs', type=int, default=5,
                        help='Number of runs (default: 5)')
    parser.add_argument('--num_threads', type=int, default=4,
                        help='Number of threads (default: 4)')
    parser.add_argument('--exec_name', type=str, default='page_rank.exe',
                        help='Name of the executable (default: page_rank.exe)')
    parser.add_argument('--test_folder_path', type=str, default='',
                        help='Path to the folder containing test files (e.g. page_rank/tests))')
    parser.add_argument('--test_sets', nargs='+', required = True,
                        help='List of test file names (e.g. facebook.txt, p2p-Gnutella24.txt))')

    # Parse the arguments
    return parser.parse_args()

def main(line_args : argparse.Namespace):

    NUM_RUNS : int = line_args.num_runs #default value is 5
    NUM_THREADS : int = line_args.num_threads #default value is 4
    EXEC_NAME : str = line_args.exec_name #default value is page_rank.exe

    test_folder_path: str = line_args.test_folder_path #default value is empty string

    test_sets : list[str] = line_args.test_sets #default value is empty list


    args_sets : list[list] = [[test_folder_path + "/" + test, str(thread_count), "-m", "1"] for test in test_sets for thread_count in range(1, NUM_THREADS + 1)]

    # Checks if the executable exists, else runs make
    try:
        subprocess.run([f"./{EXEC_NAME}"], capture_output=True) # Even if the executable exists, it will just invoke a message 
                                                                # saying that the user needs to provide arguments
    except FileNotFoundError:
        subprocess.run(["make"])

    results : dict[str, dict[str, dict[str, float]]] = {}
    for args in args_sets:
        times = []
        for i in range(NUM_RUNS):
            start_time = time.time()
            subprocess.run([f"./{EXEC_NAME}", *args], capture_output=True)
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

if __name__ == "__main__":
    main(arg_parser())