# Визуализация проекта «Транспортный справочник»  
## Описание

> Проект строится на основе «[Транспортного справочника](https://github.com/burakshaevn/cpp-transport-catalogue "Основной проект транспортного справочника")», добавляя визуализацию с помощью **Qt** и управление данными через **PostgreSQL**.

Проект  предоставляет средства для отображения маршрутов общественного транспорта на основе данных из базы данных PostgreSQL. Данные конвертируются в JSON-формат, после чего используется Qt и SVG для рендеринга карты с проекцией географических координат, маршрутами и остановками.

## Пример
<table>
  <tr>
    <td>
      <figure>
        <img src="https://github.com/user-attachments/assets/d9709280-7fe4-4b19-ac2a-4bb2bc221116" alt="Image 1">
        <figcaption>Рис. 1 — Отрисовка маршрутов в .SVG формат.</figcaption>
      </figure>
    </td>
    <td>
      <figure>
        <img src="https://github.com/user-attachments/assets/00987145-1964-4267-b629-210ffad3178d" alt="Image 2">
        <figcaption>Рис. 2 — Подключение к базе данных.</figcaption>
      </figure>
    </td>
    <td>
      <figure>
        <img src="https://github.com/user-attachments/assets/b3dc0c76-08b0-4c0f-baae-1d566c2d5fd9" alt="Image 3">
        <figcaption>Рис. 3 — Управление остановками.</figcaption>
      </figure>
    </td>
  </tr>
</table>

<table>
  <tr>
    <td>
    <figure>
      <img src="https://github.com/user-attachments/assets/6ced1353-d37d-4328-8002-ae607d199c7c" alt="Image 4"> 
      <figcaption>Рис. 4 — Управление маршрутами.</figcaption>
    </figure>
  </td>
  <td>
    <figure>
      <img src="https://github.com/user-attachments/assets/f16a20ce-ebe3-4c49-aa57-3fb6b0c67be2" alt="Image 5">
      <figcaption>Рис. 5 — Управление дистанциями.</figcaption>
    </figure>
    </td>
  </tr>
</table>   
 
## Сборка и запуск
Требования:
- C++17
- Qt 5.15+
- PostgreSQL 17+
- CMake 3.5+

### Подготовка
Для корректного запуска модуля QSqlDatabase, в переменных средах ПК (PATH) должен быть указан путь до бинарных библиотек Postgres. Пример: Системные переменные → PATH → `C:\dev\PostgreSQL\17\bin`.

### Создание базы данных
* В репозитории хранится файл с раширением `.sql`, содержащий резервную копию базы данных.
  
  ```
  1. Выполняем авторизацию в pgAdmin 4.
  2. Создаём базу данных, в которой будут расположены таблицы, триггеры и тд (она должна называться transport_catalogue)
  3. Открываем «запросник» Query Tool → Вставить в запросник код из файла .sql → Выполняем запрос.
  ```

### Сборка под редакторы

* Qt Creator
  ```
  1. Распаковать репозиторий.
  2. Open Qt Creator → Open Project.
  3. В открывшемся окне указываем путь к распакованному репозиторию и выбираем файл CMakeLists.txt.
  4. Выполняем конфигурацию проекта под нужный компилятор «Configure».
  ```

* Visual Studio 2022
  ```sh
  cd ..\qt-transport-catalogue\
  mkdir build-vs
  cd ..\qt-transport-catalogue\build-vs
  cmake .. -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:\Qt\6.6.2\msvc2019_64\"
  mingw32-make
  ```
  где `-DCMAKE_PREFIX_PATH="C:\Qt\6.6.2\msvc2019_64\"` — путь к месту установки фреймворка Qt, собранного с использованием компилятора Microsoft Visual Studio.
 
## ER-диаграмма базы данных 
<div align="center">
  <img src="https://github.com/user-attachments/assets/ab7bf03a-d735-459d-b5a1-687a030b3099" alt="image">
  <p>Рис. 6 — ER-диаграмма.</p>
</div> 
