# FastLogParser

*High-performance multithreaded C++ log analyzer*

## About the Project
This project is a demonstration of systems programming skills and multithreading in modern C++. 
The application reads massive log files, efficiently distributes the workload across all available CPU cores, and calculates statistics based on error types (INFO, WARNING, ERROR, FATAL).

## Benchmarks and Performance
The program is capable of processing a **~600 MB log file (10,000,000 lines)** in just over **1 second**!

```plaintext
ERROR 2500129
WARNING 2499994
INFO 2499964
FATAL 2499913
Lead time(parsing): 1288183 microseconds

Process finished with exit code 0
```

## Technical Solutions

* **Producer-Consumer Architecture:** The system is divided into two distinct tasks. A single thread (Producer/Reader) sequentially reads the log file and forms batches of strings. A pool of threads (Consumers) asynchronously pulls these batches from the queue and parses them in parallel. This prevents I/O blocking and maximizes CPU utilization.

* **Synchronization (Condition Variables):** Instead of inefficient busy-waiting, consumer threads "sleep" when the queue is empty. Once the Reader forms a new batch of data, it instantly wakes up one of the idle threads using `cv_.notify_one()`. This minimizes CPU resource consumption during idle times.

* **Lock Contention (Local Aggregation):** To avoid the bottleneck of multiple threads simultaneously writing results to a shared map, each Consumer accumulates statistics in its own local hash table (`local_error`). The global mutex (`mapMutex_`) is locked only once per thread — during the final merging of local results.

* **Memory Optimization (std::string_view & std::move):** The transfer of generated batches between threads is done via move semantics (`std::move`), which completely eliminates deep copying of string arrays. During message parsing, `std::string_view` is used to determine the error type (INFO, ERROR, etc.) without unnecessary memory allocations for substrings.

## How to Run the Project

### 1. Generating Test Data
The project includes a script to create heavy log files.
1. In CMake, select the `generator` target.
2. Compile and run it.
3. A `test.log` file with N lines (depending on the specified number of iterations) will appear in the project root (or in the cmake-build-debug / cmake-build-release folder).

### 2. Running the Parser
1. It is highly recommended to switch the build profile to **Release**, though Debug works as well.
2. Select the main parser target.
3. Run the application.

## Stack
* C++20
* CMake
* Libraries: `<thread>`, `<mutex>`, `<condition_variable>`, `<string_view>`, `<unordered_map>`.


## ---------------------------------------------------


# FastLogParser ->
*Високопродуктивний багатопотоковий аналізатор логів на C++*

## Про проект ->
Цей проект – демонстрація навичок системного програмування та роботи з багатопоточністю у сучасному C++. 
Програма читає об'ємні лог-файли, ефективно розподіляє навантаження на всі доступні ядра процесора і підраховує статистику за типами помилок (INFO, WARNING, ERROR, FATAL).

## Бенчмарки і продуктивність ->
Програма здатна обробити лог-файл розміром **~600 МБ (10 000 000 рядків)** трохи більше, ніж за **1 секунду**!

![Результат парсингу у режимі Release](
```Plaintext
ERROR 2500129
WARNING 2499994
INFO 2499964
FATAL 2499913
Lead time(parsing): 1288183 microseconds

Process finished with exit code 0
```
)  

## Технічні рішення ->

* **Архітектура Producer-Consumer:** [ Система розділена на два типи задач. Один потік ( Reader) виконує послідовне читання лог-файлу та формує батчі рядків. Група потоків (Consumers) асинхронно забирає ці батчі з черги та розбирає їх паралельно. Це дозволяє уникнути блокування I/O операцій та максимізувати утилізацію процесора. ]

* **Синхронізація (Condition Variables):** [ Замість неефективного циклічного опитування (busy-waiting), потоки-споживачі "засинають", коли черга порожня. Reader, сформувавши новий батч даних, миттєво будить один із вільних потоків за допомогою {cv_.notify_one().} Це мінімізує споживання ресурсів CPU в режимі очікування. ]

* **Lock Contention (Локальна агрегація):** [ Щоб уникнути ефекту одночасного запису результатів у спільну мапу різними потоками, кожен Consumer накопичує статистику у власній локальній хеш-таблиці (local_error). Глобальний м'ютекс (mapMutex_) блокується лише один раз для кожного потоку — під час фінального злиття локальних результатів. ]

* **Оптимізація пам'яті  (std::string_view & std::move):** [ Передача сформованих батчів між потоками відбувається через семантику переміщення (std::move), що повністю виключає глибоке копіювання масивів рядків. Під час розбору повідомлень використовується std::string_view для визначення типу помилки (INFO, ERROR тощо) без зайвих алокацій пам'яті для підрядків. ]

## --- > Як запустити проект --->

### 1. Генерація тестових даних
У проекті передбачено скрипт для створення важких логів.
1. У CMake виберіть таргет `generator`.
2. Скомпілюйте та запустіть його.
3. У корені проекту (або в папці cmake-build-debug / cmake-build-release) з'явиться файл test.log на N-кількість рядків.(залежно від заданої кількості ітерацій)

### 2. Запуск парсеру
1. Краще переключити профіль складання в **Release**, але можна і через Debug.
2. Виберіть основний таргет парсера.
3. Запустіть програму.

## Стек ->
* C++20
* CMake
* Бібліотеки: `<thread>`, `<mutex>`, `<condition_variable>`, `<string_view>`, `<unordered_map>`.
