// DAST for Marginália, the bookstore software: runs the binary with hostile inputs and
// watches how it behaves, without looking at the source code. Each case describes
// the input and what the program is expected to do.
//
// Usage: build/hostile_cases build/marginalia

# include <cerrno>
# include <csignal>
# include <cstdio>
# include <cstring>
# include <string>
# include <vector>

# include <poll.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <time.h>
# include <unistd.h>

// Commands that set up a valid session before each attack.
static const std::string SETUP =
    "customer 7 10 Ana Souza\n"
    "author 1 M Machado de Assis\n"
    "book 1 1 39.90 Dom Casmurro\n";

// NO_CRASH only requires the program to finish normally, and REJECT also requires
// the answer to the last command to start with "Error".
enum Expectation { NO_CRASH, REJECT };

struct Case
{
    std::string name;
    std::string commands;
    Expectation expectation;
};

struct Run
{
    bool finished;
    int status;
    std::vector<std::string> answers;
};

static const int TIMEOUT_MS = 10000;

static std::string fullInvoice()
{
    std::string commands = SETUP + "add 1 1\n";

    for (int n = 2; n <= 11; n++)
    {
        commands += "book " + std::to_string(n) + " 1 10.00 Book " + std::to_string(n) + "\n";
        commands += "add " + std::to_string(n) + " 1\n";
    }

    return commands;
}

static std::vector<Case> cases()
{
    return {
        {"5,000-character title", SETUP + "book 2 1 10.00 " + std::string(5000, 'A') + "\ninvoice\n", NO_CRASH},
        {"negative quantity", SETUP + "add 1 -5\n", REJECT},
        {"zero quantity", SETUP + "add 1 0\n", REJECT},
        {"eleventh book in a full invoice", fullInvoice(), REJECT},
        {"text instead of a quantity", SETUP + "add 1 five\n", REJECT},
        {"quantity larger than an int", SETUP + "add 1 99999999999999999999\n", REJECT},
        {"book that does not exist", SETUP + "add 42 1\n", REJECT},
        {"unknown command", SETUP + "delete everything\n", REJECT},
    };
}

static long elapsedMs(const timespec &start)
{
    timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_sec - start.tv_sec) * 1000 + (now.tv_nsec - start.tv_nsec) / 1000000;
}

// Runs the binary with the given input and collects its exit status and its answers.
static Run run(const char *binary, const std::string &commands)
{
    Run result = {false, 0, {}};
    int input[2];
    int output[2];

    if (pipe(input) != 0 || pipe(output) != 0)
    {
        perror("pipe");
        return result;
    }

    pid_t child = fork();

    if (child == 0)
    {
        dup2(input[0], STDIN_FILENO);
        dup2(output[1], STDOUT_FILENO);
        close(input[0]);
        close(input[1]);
        close(output[0]);
        close(output[1]);
        execl(binary, binary, (char *) NULL);
        _exit(127);
    }

    close(input[0]);
    close(output[1]);
    signal(SIGPIPE, SIG_IGN);

    size_t written = 0;

    while (written < commands.size())
    {
        ssize_t n = write(input[1], commands.data() + written, commands.size() - written);

        if (n <= 0)
        {
            break;
        }

        written += (size_t) n;
    }

    close(input[1]);

    std::string text;
    char buffer[4096];
    timespec start;
    bool closed = false;

    clock_gettime(CLOCK_MONOTONIC, &start);

    while (!closed && elapsedMs(start) < TIMEOUT_MS)
    {
        pollfd fd = {output[0], POLLIN, 0};

        if (poll(&fd, 1, 100) > 0)
        {
            ssize_t n = read(output[0], buffer, sizeof buffer);

            if (n <= 0)
            {
                closed = true;
            }
            else
            {
                text.append(buffer, (size_t) n);
            }
        }
    }

    close(output[0]);

    if (!closed)
    {
        kill(child, SIGKILL);
        waitpid(child, &result.status, 0);
        return result;
    }

    waitpid(child, &result.status, 0);
    result.finished = true;

    size_t begin = 0;

    while (begin < text.size())
    {
        size_t end = text.find('\n', begin);

        if (end == std::string::npos)
        {
            end = text.size();
        }

        result.answers.push_back(text.substr(begin, end - begin));
        begin = end + 1;
    }

    return result;
}

// Evaluates one case and returns a failure message, or an empty text when the program behaved well.
static std::string evaluate(const char *binary, const Case &test)
{
    Run result = run(binary, test.commands);

    if (!result.finished)
    {
        return "the program did not finish within 10 seconds";
    }

    if (WIFSIGNALED(result.status))
    {
        return "the program crashed (signal " + std::to_string(WTERMSIG(result.status)) + ")";
    }

    if (WEXITSTATUS(result.status) != 0)
    {
        return "the program ended with an error (exit code " + std::to_string(WEXITSTATUS(result.status)) + ")";
    }

    if (test.expectation == REJECT && (result.answers.empty() || result.answers.back().rfind("Error", 0) != 0))
    {
        std::string last = result.answers.empty() ? "(none)" : result.answers.back();
        return "the hostile command was accepted (answer: " + last + ")";
    }

    return "";
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <Marginália binary>\n", argv[0]);
        return 2;
    }

    std::vector<Case> all = cases();
    int failures = 0;

    for (const Case &test : all)
    {
        std::string reason = evaluate(argv[1], test);

        if (reason.empty())
        {
            printf("[ok] %s\n", test.name.c_str());
        }
        else
        {
            printf("[FAILED] %s: %s\n", test.name.c_str(), reason.c_str());
            failures++;
        }
    }

    printf("%d of %zu hostile cases failed\n", failures, all.size());
    return failures == 0 ? 0 : 1;
}
