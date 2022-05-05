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

### Формат ввода

Входной файл представляет из себя словаарь в JSON формате со следующими элементами:
```JSON
{
"render settings" : // настройки отрисовки SVG
{
        "width": 4700, //ширина холста
        "height": 3200, // высота холста
        "padding": 50, // отступ от краёв
        "stop_radius": 4, // радиус круга-маркера остановки
        "line_width": 8, // толщина линий маршрутов
        "stop_label_font_size": 10, // размер шрифта наазваний остановок
        "stop_label_offset": [5, 5], // смещение названия остановки
        "underlayer_color": "white", // цвет подложки текста
        "underlayer_width": 3, // ширина подложки текста
        "color_palette": [ [0, 120, 191], [172, 191, 227, 0.54], "white"], // палитра цветов в RGB, RGBA или текстовом формате
        "bus_label_font_size": 14, // размер шрифта названия маршрута
        "bus_label_offset": [10, -15] // смещение названия маршрута
        }

"base_requests": // запросы на добавление в базу данных остановок и маршрутов
[ 
        { // запрос на добавление остановки
                "type": "Stop", // тип запроса
                "name": "Aeroport",
                "latitude": 55.79981,
                "longitude": 37.53412
        }, 

        { // запрос на добавление маршрута
                "type": "Bus", // тип запроса
                "name": "Sokol'nicheskaya liniya",
                "stops": ["Ulica Podbel'skogo", "CHerkizovskaya", "Preobrazhenskaya ploshchad'"],
                "is_roundtrip": false // true если кольцевой маршрут
        } 
],

"stat_requests": //запросы к базе данных
[ 
        { 
        "id": 1,
        "type": "Bus",
        "name": "Butovskaya liniya"
        }, 
        
        {
        "id": 2,
        "type": "Stop",
        "name": "Kon'kovo"
        }, 
        
        {
        "id": 3,
        "type": "Map"
        }
]
}
```
