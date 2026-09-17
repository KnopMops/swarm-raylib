# Swarm

Учебный 2D-шутер с видом сверху, написанный на C++ с использованием библиотеки raylib.

## 📋 Требования

- Windows 10/11
- [Visual Studio Code](https://code.visualstudio.com/)
- [Raylib Installer](https://www.raylib.com/) (версия 5.x, для MinGW)

## 🛠 Установка окружения

### Шаг 1. Установка Raylib

1. Перейдите на официальный сайт https://www.raylib.com/
2. Нажмите кнопку **Download Now**.
3. В разделе для Windows скачайте **Raylib Installer** (файл вида `raylib_installer_v5.x.mingw.exe`).
4. Запустите инсталлятор. На всех этапах можно оставить настройки по умолчанию и просто нажимать **Next** → **Install**.
5. По умолчанию всё установится в `C:\raylib\`. Внутри будут:
   - `C:\raylib\raylib\` — сама библиотека (заголовки и `.a`-файлы)
   - `C:\raylib\w64devkit\` — компилятор MinGW-w64

> **Важно:** запомните путь, куда установился raylib. В примерах ниже используется `C:/raylib/`, но если вы выбрали другую папку — замените путь во всех конфигурационных файлах.

### Шаг 2. Добавление компилятора в PATH

Чтобы `make` и `g++` были доступны из терминала, добавьте путь к компилятору в переменную среды:

1. Нажмите `Win`, введите **«Изменение переменных среды текущего пользователя»** и откройте это окно.
2. В разделе **«Переменные среды пользователя»** выберите переменную `Path` и нажмите **«Изменить»**.
3. Нажмите **«Создать»** и добавьте путь: `C:\raylib\w64devkit\bin`.
4. Нажмите **OK** во всех открытых окнах.

### Шаг 3. Установка расширений VS Code

Откройте VS Code и установите два расширения (нажмите `Ctrl+Shift+X` и введите в поиске):

| Расширение     | Идентификатор              |
| -------------- | -------------------------- |
| C/C++          | `ms-vscode.cpptools`       |
| Makefile Tools | `ms-vscode.makefile-tools` |

## ⚙️ Настройка проекта (.vscode)

В корне проекта должна быть папка `.vscode/`. Создайте в ней три файла с указанным ниже содержимым. **Если raylib установлен не в `C:/raylib/`, обязательно замените путь в каждом файле на свой.**

### c_cpp_properties.json

Отвечает за работу IntelliSense (подсказки, автодополнение, переход к определениям). Здесь указываем компилятор, пути к заголовкам raylib и стандарт C++.

```json
{
	"configurations": [
		{
			"name": "Win32",
			"includePath": [
				"${workspaceFolder}/**",
				"C:/raylib/raylib/src/**",
				"C:/raylib/raylib/src/external/**"
			],
			"defines": ["_DEBUG", "UNICODE", "_UNICODE"],
			"compilerPath": "C:/raylib/w64devkit/bin/g++.exe",
			"cStandard": "c11",
			"cppStandard": "c++17",
			"intelliSenseMode": "windows-gcc-x64"
		}
	],
	"version": 4
}
```

**Что здесь важно:**

- `includePath` — папки, в которых IntelliSense ищет заголовочные файлы. Путь `C:/raylib/raylib/src/**` подключает заголовки raylib.
- `compilerPath` — путь к компилятору `g++.exe` из комплекта w64devkit.
- `cppStandard` — стандарт C++, используемый в проекте.

### tasks.json

Определяет задачи сборки. Задача с именем `build debug` будет вызываться при нажатии F5, задача `clean` — для очистки.

```json
{
	"version": "2.0.0",
	"tasks": [
		{
			"label": "build debug",
			"type": "shell",
			"command": "make",
			"args": [
				"PROJECT_NAME=Swarm",
				"RAYLIB_PATH=C:/raylib/raylib",
				"MODE=DEBUG"
			],
			"options": {
				"cwd": "${workspaceFolder}"
			},
			"problemMatcher": ["$gcc"],
			"group": {
				"kind": "build",
				"isDefault": true
			},
			"detail": "Сборка отладочной версии с отладочными символами"
		},
		{
			"label": "build release",
			"type": "shell",
			"command": "make",
			"args": [
				"PROJECT_NAME=Swarm",
				"RAYLIB_PATH=C:/raylib/raylib",
				"MODE=RELEASE"
			],
			"options": {
				"cwd": "${workspaceFolder}"
			},
			"problemMatcher": ["$gcc"],
			"group": {
				"kind": "build",
				"isDefault": false
			},
			"detail": "Сборка оптимизированной версии"
		},
		{
			"label": "clean",
			"type": "shell",
			"command": "make",
			"args": ["clean", "RAYLIB_PATH=C:/raylib/raylib"],
			"options": {
				"cwd": "${workspaceFolder}"
			},
			"problemMatcher": []
		}
	]
}
```

**Что здесь важно:**

- `RAYLIB_PATH` — путь к папке raylib (без `/src` на конце — Makefile сам подставит нужный подпуть).
- `MODE=DEBUG` — собирает исполняемый файл с отладочными символами для пошаговой отладки.
- `"isDefault": true` у задачи `build debug` — именно она запускается при нажатии F5.

### launch.json

Определяет конфигурацию запуска и отладки. Здесь указываем, какой исполняемый файл запускать и как к нему подключаться.

```json
{
	"version": "0.2.0",
	"configurations": [
		{
			"name": "Debug (GDB)",
			"type": "cppdbg",
			"request": "launch",
			"program": "${workspaceFolder}/build/Swarm.exe",
			"args": [],
			"stopAtEntry": false,
			"cwd": "${workspaceFolder}",
			"environment": [],
			"externalConsole": false,
			"MIMode": "gdb",
			"miDebuggerPath": "C:/raylib/w64devkit/bin/gdb.exe",
			"setupCommands": [
				{
					"description": "Включить pretty-printing для STL",
					"text": "-enable-pretty-printing",
					"ignoreFailures": true
				}
			],
			"preLaunchTask": "build debug"
		}
	]
}
```

**Что здесь важно:**

- `program` — путь к собираемому `.exe`-файлу. Должен совпадать с именем, которое задано в Makefile (`PROJECT_NAME`).
- `miDebuggerPath` — путь к отладчику `gdb.exe` из комплекта w64devkit.
- `preLaunchTask` — имя задачи из `tasks.json`, которая выполнится перед запуском (автоматическая сборка).

## ▶️ Сборка и запуск

1. Откройте папку проекта в VS Code: **File → Open Folder**.
2. Убедитесь, что выбран компилятор из w64devkit. Для этого нажмите `Ctrl+Shift+P`, введите `C/C++: Select a Configuration` и выберите **Win32**.
3. Нажмите **F5** — VS Code автоматически:
   - выполнит задачу `build debug` (соберёт проект через `make`);
   - запустит `gdb.exe` и откроет игру в режиме отладки.

Если всё настроено правильно, откроется окно игры «Swarm».

### Ручная сборка (альтернатива)

Если нужно собрать без отладки:

```bash
# Откройте терминал (Ctrl + `) и выполните:
make PROJECT_NAME=Swarm RAYLIB_PATH=C:/raylib/raylib MODE=RELEASE
```

Исполняемый файл появится в папке `build/`.

## 🎮 Управление в игре

| Клавиша   | Действие                              |
| --------- | ------------------------------------- |
| `W A S D` | Движение                              |
| `Мышь`    | Прицеливание                          |
| `ЛКМ`     | Стрельба                              |
| `L`       | Заспавнить новую волну врагов         |
| `F1`      | Показать/скрыть отладочную информацию |
| `F2`      | Убить всех врагов                     |
| `Escape`  | Показать/скрыть курсор                |
| `R`       | Возродиться (после смерти)            |
| `M`       | Выйти в главное меню                  |

## 🐛 Возможные проблемы

**«raylib.h: No such file or directory»**

- Проверьте путь в `c_cpp_properties.json` (`includePath`) и в `tasks.json` (`RAYLIB_PATH`). Путь должен указывать на реальную папку, куда установлен raylib.

**«make: command not found»**

- Убедитесь, что путь `C:\raylib\w64devkit\bin` добавлен в переменную `Path` и терминал перезапущен после изменения.

**«Не удалось запустить программу: файл не найден»**

- Проверьте поле `program` в `launch.json` — имя `.exe`-файла должно совпадать с `PROJECT_NAME` в `tasks.json`.

**IntelliSense не видит функции raylib**

- Перезагрузите VS Code (`Ctrl+Shift+P` → **Developer: Reload Window**).
- Убедитесь, что в `compilerPath` указан существующий `g++.exe`.

## 📚 Полезные ссылки

- [Официальный сайт raylib](https://www.raylib.com/)
- [Официальный гайд по raylib в VS Code](https://github.com/raysan5/raylib/wiki/Using-raylib-in-VSCode)
- [Шаблон raylib-quickstart (альтернативная настройка)](https://github.com/raylib-extras/raylib-quickstart)
- [Документация raylib](https://www.raylib.com/cheatsheet/cheatsheet.html)

## 📄 Лицензия

Этот проект распространяется под лицензией MIT. См. файл [LICENSE](LICENSE).

```
MIT License

Copyright (c) 2026 KnopMops

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
