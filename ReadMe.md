## v2
Эксперимент по эффективному хранению и декомпрессии url-ов из access-логов.

### Мотивация:
1. Access-логи занимают очень много места (~30гб/файл) даже при эффективном сжатии.
1. Воронки выполняют рассчеты на основе семпла всего в пару процентов от access-логов.

Поскольку из-за этого рассчеты происходят медленно и не точно, то планируется ускорить воронки за счет более эффективного формата хранения access-логов, оптимизированного для быстрого чтения (больше url-ов за одно обращение к диску). 

### Цель:
Текущий эксперимент по результатам сравнения скорости декомпрессии предварительно обработанных url-ов, должен показать, что лучше использовать для ускорения воронок, своё решение на C++ или ClickHouse.

### Алгоритм компрессии:
1. Для упрощения эксперимента отбрасываем из записей все, кроме пути из url.
1. Сопоставляем каждому уникальному терму из путей свой уникальный id. Чем чаще повторяется терм, тем меньше его id.
1. Кодируем получившиеся из каждого пути последовательности цифр с помощью variable int encoding (старший бит хранит знание, является ли байт последним в числе).

### Как собирать?
В директории репозитория:
```
cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -B cmake-build-release
```

##### Логи для эксперимента: 
`vault.bazadev.net:/data/logs/logs/nginx-frontend/`