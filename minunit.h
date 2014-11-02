/* file: minunit.h */
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)
#define mu_time_function(testFunction, timeVar) do { clock_t start = clock(); testFunction; timeVar += clock()-start;}while(0)
extern int tests_run;