/** @file trace_log.h
 *
 * This file contains all functions which interact directly
 * with the trace.log file.
 *
 */

/** @brief Adds a trace line to the trace log.
 *  @param text The trace line.
 *  @return
 */
static void addTrace(const char *text){
    FILE *f = fopen("/var/log/trace.log", "a+");
    if (f == NULL)
    {
        printf("Error opening trace log file!\n");
    }

    fprintf(f, "%s\n", text);
    fclose(f);
}