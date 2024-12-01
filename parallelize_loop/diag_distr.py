N = 100
P = 10

def make_schedule(N, P):

    worksize = int((N**2 + (P-1)) / P)
    print(f'worksize: {worksize}')

    schedule = []
    last_diag = 0
    cum_size = 0

    for i in range(P-1):
        current_size = 0

        for k in range(last_diag + 1, 2 * N - 2):

            diag_size = k

            if (k > N):
                diag_size = 2*N - k

            print(diag_size)

            current_size = current_size + k
        
            cur_worksize = ((N**2 - cum_size) / (P - i))
            if (current_size > cur_worksize):
                schedule.append([last_diag, k, current_size])
                last_diag = k
                break

        cum_size = cum_size + current_size

    print(cum_size)
    schedule.append([last_diag, 2*N-1, N**2 - cum_size])

    return schedule

schedule = make_schedule(N, P)
print(schedule)
