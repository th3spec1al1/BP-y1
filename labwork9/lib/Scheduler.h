#pragma once

#include <vector>
#include <algorithm>
#include <tuple>
#include "SomeType.h"

namespace Scheduler {

    class TTaskScheduler {

    private:

        class BaseTask {

        public:
            virtual ~BaseTask() = default;
            virtual void Call() = 0;
            virtual bool IsExecuted() = 0;
            virtual SomeType GetResult() = 0;
            virtual size_t GetId() = 0;

        };

        template <typename T>
        class FutureResult {

        public:

            FutureResult(std::shared_ptr<BaseTask> ptr) : ptr_(ptr) {}

            operator T() { return  ptr_->GetResult().Cast<T>(); }

        private:

            std::shared_ptr<BaseTask> ptr_;

        };

        template <typename Func, typename... Args>
        class Task : public BaseTask {

        public:

            Task(Func function, size_t id, Args... args)
            : function_(function), args_(std::make_tuple(std::forward<Args>(args)...)), id_(id), is_executed_(false) {}

            void Call() override { result_ = std::apply(function_, args_); }

            bool IsExecuted() override { return is_executed_; }

            SomeType GetResult() override { return result_; }

            size_t GetId() override { return id_; }

            std::tuple<Args...> args_;
            Func function_;
            size_t id_;
            SomeType result_;
            bool is_executed_;

        };

        void DFS(size_t vertex, std::vector<bool>& visited) {
            visited[vertex] = true;
            for (auto u : graph_[vertex]) {
                if (!visited[u]) {
                    DFS(u, visited);
                }
            }
            sort_order_.push_back(vertex);
        }

        void Sort() {
            std::vector<bool> visited(next_id_);
            sort_order_.clear();
            for (size_t i = 0; i < next_id_; ++i) {
                if (!visited[i]) {
                    DFS(i, visited);
                }
            }
            is_sorted_ = true;
        }

        std::vector<std::shared_ptr<BaseTask>> tasks_;
        std::vector<std::vector<size_t>> graph_;
        std::vector<size_t> sort_order_;
        size_t next_id_ = 0;
        bool is_sorted_ = false;

    public:

        template <typename Func, typename... Args>
        auto add(Func func, Args... args) {
            std::cout << "...\n";
            auto task = std::make_shared<Task<Func, Args...>>(func, ++next_id_, std::forward<Args>(args)...);
            std::cout << "...\n";
            graph_.push_back(std::vector<size_t>{});
            sort_order_.push_back(next_id_ - 1);
            tasks_.push_back(task);
            return (next_id_ - 1);
        }

        template <typename T>
        FutureResult<T> getFutureResult(size_t id) {
            graph_[id].push_back(next_id_);
            is_sorted_ = false;
            return FutureResult<T>(tasks_[id]);
        }

        template <typename T>
        T getResult(size_t id) {
            auto task = tasks_[id];
            if (!task->IsExecuted()) {
                executeAll();
            }

            return task->GetResult().Cast<T>();
        }

        void executeAll() {
            if (!is_sorted_) {
                Sort();
            }
            for (int i = sort_order_.size() - 1; 0 <= i; --i) {
                if (!tasks_[sort_order_[i]]->IsExecuted()) {
                    tasks_[sort_order_[i]]->Call();
                }
            }
        }

    };
    
} 