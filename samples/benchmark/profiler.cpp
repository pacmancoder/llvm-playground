#include <string_view>
#include <iostream>
#include <shared_mutex>
#include <thread>
#include <map>
#include <vector>

namespace {

    struct FunctionProfilingData {
        long long lastEntryTime;
        long long accumulatedTime;

        size_t currentEntryNesting;
        size_t entryCount;
    };


    using FileName = const char*;
    using FunctionName = const char*;
    using FunctionsProfilingData = std::map<FunctionName, FunctionProfilingData>;
    using FilesProfilingData = std::map<FileName, FunctionsProfilingData>;
    using ThreadsProfilingData = std::map<std::thread::id, FilesProfilingData>;


    using FilesProfilingResult = std::map<FileName, long long>;
    using FunctionsProfilingResults = std::map<FunctionName, FilesProfilingResult>;
    using SortedTimings = std::multimap<long long, FunctionsProfilingResults::iterator>;

    struct ProfilingResults {
        long long overallTime;
        FunctionsProfilingResults functionProfilingResults;
        SortedTimings sortedTimings;
    };

    class Profiler {
    public:
        static Profiler& GetInstance();

        void EnterFunction(FunctionName file, FunctionName function);
        void ExitFunction(FunctionName file, FunctionName function);

        void PrintResults(std::ostream& stream);

    public:
        Profiler(Profiler&) = delete;
        Profiler& operator=(Profiler&) = delete;

    private:
        Profiler() = default;

        FilesProfilingData& GetThreadProfilingData();
        ProfilingResults CalculateResults();

    private:
        std::shared_mutex threadsProfilingDataMutex_;
        ThreadsProfilingData threadsProfilingData_;
    };
}

static long long GetCurentTimeUs() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

Profiler& Profiler::GetInstance() {
    static Profiler profiler;
    return profiler;
}

FilesProfilingData& Profiler::GetThreadProfilingData() {
    auto threadId = std::this_thread::get_id();
    std::shared_lock lock(threadsProfilingDataMutex_);
    if (threadsProfilingData_.find(threadId) == threadsProfilingData_.end()) {
        lock.unlock();
        {
            std::unique_lock writeLock(threadsProfilingDataMutex_);
            threadsProfilingData_.emplace(threadId, FilesProfilingData());
        }
        lock.lock();
    }

    return threadsProfilingData_.at(threadId);
}

void Profiler::EnterFunction(FileName file, FunctionName function) {
    auto& threadProfilingData = GetThreadProfilingData();
    auto& fileEntry = threadProfilingData[file];
    auto& functionEntry = fileEntry[function];

    if (functionEntry.currentEntryNesting == 0) {
        functionEntry.lastEntryTime = GetCurentTimeUs();
    }
    ++functionEntry.currentEntryNesting;
    ++functionEntry.entryCount;
}

void Profiler::ExitFunction(FileName file, FunctionName function) {
    auto& threadProfilingData = GetThreadProfilingData();
    auto& fileEntry = threadProfilingData[file];
    auto& functionEntry = fileEntry[function];

    --functionEntry.currentEntryNesting;
    auto executionTime = GetCurentTimeUs() - functionEntry.lastEntryTime;

    // Very rough accumulation technique but fast :)
    functionEntry.accumulatedTime += executionTime;
}

ProfilingResults Profiler::CalculateResults() {
    auto results = ProfilingResults {};
    for (auto& [thread, fileData] : threadsProfilingData_) {
        for (auto& [fileName, functions] : fileData) {
            for (auto& [functionName, functionData] : functions) {
                auto averageTime = static_cast<long long>(
                    static_cast<double>(functionData.accumulatedTime) / functionData.entryCount);
                results.functionProfilingResults[functionName][fileName] = averageTime;
                results.overallTime += averageTime;
            }
        }

        (void) thread;
    }

    for (auto functionProfilingIt = results.functionProfilingResults.begin(); functionProfilingIt != results.functionProfilingResults.end(); ++functionProfilingIt) {
        auto& [functionName, fileData] = *functionProfilingIt;

        long long functionOverallTime = 0;
        for (auto& [fileName, time] : fileData) {
            functionOverallTime += time;
        }
        results.sortedTimings.emplace(std::pair<long long, FunctionsProfilingResults::iterator>(functionOverallTime, functionProfilingIt));
    }

    return results;
}

static std::string ToHumanReadablePercents(long long whole, long long part) {
    return std::to_string(static_cast<int>((static_cast<float>(part) / whole) * 100)) + "%";
}

static std::string ToHumanReadableTime(long long us) {
    static const long long US_IN_S = 1000 * 1000;
    static const long long US_IN_MS = 1000;

    if (us >= US_IN_S) {
        return std::to_string(us / US_IN_S) + "s";
    }

    if (us >= US_IN_MS) {
        return std::to_string(us / US_IN_MS) + "ms";
    }

    return std::to_string(us) + "us";
}

void Profiler::PrintResults(std::ostream& stream) {
    stream << "\n\n=== PROFILING RESULTS ===\n";

    auto results = CalculateResults();
    stream << "Overall execution time:\n\t" << ToHumanReadableTime(results.overallTime) << "\n";
    stream << "States by function name:\n";
    for (auto sortedTimingsIt = results.sortedTimings.rbegin(); sortedTimingsIt != results.sortedTimings.rend(); ++sortedTimingsIt) {
        auto& [functionOverallTime, function] = *sortedTimingsIt;
        if (functionOverallTime == 0) {
            continue;
        }
        auto& [functionName, functionDataByFile] = *function;
        if (functionDataByFile.size() == 1) {
            auto functionTime = functionDataByFile.begin()->second;
            stream << "\t[" << functionName << "] time: "
                   << ToHumanReadableTime(functionTime) << ", percentage: "
                   << ToHumanReadablePercents(results.overallTime, functionTime) << "\n";
        } else {
            stream << "\t[" << functionName << "] time: "
                   << ToHumanReadableTime(functionOverallTime) << ", percentage: "
                   << ToHumanReadablePercents(results.overallTime, functionOverallTime) << "\n";
            for (auto [file, time] : functionDataByFile) {
                stream << "\t\t[" << file << "] time: "
                       << ToHumanReadableTime(time) << ", relative percentage: "
                       << ToHumanReadablePercents(functionOverallTime, time) << "\n";
            }
        }
    }
}

// === Display api ===

extern "C" void profiler_print_results() {
    Profiler::GetInstance().PrintResults(std::cout);
}


// === Profiler pass callbacks ===

extern "C" void profiler_enter_function(const char* sourceFileName, const char* functionName) {
    Profiler::GetInstance().EnterFunction(sourceFileName, functionName);
}

extern "C" void profiler_exit_function(const char* sourceFileName, const char* functionName) {
    Profiler::GetInstance().ExitFunction(sourceFileName, functionName);
}