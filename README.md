# NanoC-Compiler

#### Очень ужасный компилятор + вирт машина, т.к большая часть кода была написана за 1 - 2 недели<br>

Данный компилятор производит компиляцию не в машинный код, а в байт-код для вирт. машины, но в любой момент
можно добавить компиляцию в машииный код, написав соответсвующий класс\
Основной смысл компилятора - написать C-подобный язык для скриптинга

\
Сейчас, в свободное время, производится рефакторинг кода
### Процесс изменения компилятора:
* Произведён частичный рефакторинг лексера
* Произведён частичный рефакторинг парсера
* Написан препроцессор (только #define, #undef)


## Реализованно:
* ### Препроцессор:
* * #define (без вложенных define)
* * #undef

* ### Лексер:
* *
