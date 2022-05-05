# Transport Catalogue

Проект транспортного каталога, позволяющий загружать в базу данных маршруты и остановки, через которые они следуют в JSON формате. После чего возможно обрабатывать запросы для получения информации о маршрутах, остановках и отрисовки схемы транспорта в SVG формате.

![Transport Catalogue](https://sun9-57.userapi.com/s/v1/ig2/tp7gXj6DritAOclhH4D7G-PeaDVV_RZ-DA6qCmG8Iy-cv4F-YjS-H5a6GFlza41tcY3zz5lNBbH-cneTQZTgk_1F.jpg?size=1632x915&quality=96&type=album "Transport Catalogue")


## Сборка и запуск

Для сборки понадобится cmake версии не ниже 3.1, clang 11.0.0, желательно LLVM.

После скачивания репозитория необходимо сделать следующее:

-Нажаать WIN + R;
-Набрать cmd в появившемся окне и нажать OK.

Ввести в консоль команды в таком порядке:
- cd <Директория с проектом>/cpp-transport-catalogue
- cmake .
- cmake --build . --target TransportCatalogue --config Release
- "Release/TransportCatalogue.exe" input.json output.json map.svg

После этого программа отработает на тестовом файле input.json и в папке проекта появятся два файла:

output.json - JSON ответ на запросы;
map.svg - векторный файл с SVG разметкой, который можно открыть в любом современном браузере.

## Подробнее
