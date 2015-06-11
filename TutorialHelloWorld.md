# Требования #
Для освоения этого урока, необходимо установить Isilme SDK и ознакомиться с концепцией системы(см. [Введение](Introduction.md) )

# Подготовка #
В Visual Studio 2010 создайте пустой проект оконного приложения Win32.
В целевых директориях проекта Debug и Release должны быть размещены динамические библиотеки, входящие в состав SDK. Для отладки используются файлы с суффиксом _d_

# Каркас приложения #
Рекоммендуется предекларировать все пользовательские классы и прототипы функций в одном или нескольких специальных заголовочных файлах.
В файле опреденеий опишем два требуемых класса: класс приложения и класс состояния игры. В этом же файле подключим SDK. Для подключения базовой функциональности достаточно подключить файл Isilme.h

Definitions.h
```

#include <Engine/Isilme.h>

class MyGame;
class PlayState;

```

Класс приложения должен лишь инициализировать начальное состояние игры, в дальнейшем он нам не понадобится

MyGame.h
```

#include "Definitions.h"
class MyGame: public HGEGame
{
public:
MyGame();
virtual void Init() override;
}
```

Класс состояния игры создадим пустым

PlayState.h
```

#include "Definitions.h"
class PlayState : public State
{
public:
PlayState();
virtual ~PlayState();
};
```

Теперь реализуем пустое состояние игры
PlayState.cpp
```

#include "PlayState.h"
PlayState::PlayState()
{
}

PlayState::~PlayState()
{
}
```

Реализуем приложение
MyGame.cpp
```


#include "MyGame.h"
#include "PlayState.h"
MyGame::MyGame()
{
}
void MyGame::Init()
{
// Создаем состояние
StatePtr state(new PlayState());

// Помещаем состояние в стек
GetStateManager()->Push(playState);
};
```

В точке входа, в функции main или WinMain, для консольного или оконного приложения соответственно необходимо создать экземпляр класса MyGame и запустить игрой цикл

main.cpp
```

#include "Definitions.h"
#include "MyGame.h"
int main()
{
MyGame game;
game.Init();
game.Start();
}
```

Каркас приложения написан, прежде чем продолжить, убедитесь, что написанный код компилируется и запускается. При запуске должна появиться заставка HGE, после которой загрузится пустое состояние игры(черный экран).
# Графический интерфейс #
Добавляем графический интерфейс