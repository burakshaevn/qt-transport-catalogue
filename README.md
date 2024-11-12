# Визуализация проекта «Транспортный справочник»  
## Описание

> Проект строится на основе «[Транспортного справочника](https://github.com/burakshaevn/cpp-transport-catalogue "Основной проект транспортного справочника")», добавляя визуализацию с помощью **Qt** и управление данными через **PostgreSQL**.

Проект  предоставляет средства для отображения маршрутов общественного транспорта на основе данных из базы данных PostgreSQL. Данные конвертируются в JSON-формат, после чего используется Qt и SVG для рендеринга карты с проекцией географических координат, маршрутами и остановками.

## Пример
<table>
  <tr>
    <td><img src="https://github.com/user-attachments/assets/3152ea73-e591-4a6f-869f-c314a50ff37f" alt="Image 1"></td>
    <td><img src="https://github.com/user-attachments/assets/b6df9822-4f91-4f41-a8a9-526bc4ce2d28" alt="Image 2"></td>
    <td><img src="https://github.com/user-attachments/assets/82ac9366-a54e-4cf3-ac61-d241a67d0d5d" alt="Image 3"></td>
  </tr>
</table>

## Сборка и запуск
Требования:
- C++17
- Qt 5.15+
- CMake 3.5+

Сборка и запуск:

* Qt Creator
  ```sh
  ...
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

## Основные классы
...

## База данных
* `routing_settings` содержит настройки маршрутизации: максимально разрешённая скорость маршрута и время ожидания на остановках.
  
<center>
    <img src="https://github.com/user-attachments/assets/ab7bf03a-d735-459d-b5a1-687a030b3099">
</center>
  
