После первого прогона видоса сбрасывать видо в начало.

Добавить счетчик фреймов во время проигрывания, чтобы знать номер текущего фрейма.
Добавить переменную desired_frame - номер фрейма, к оторому хотим перейти

Кнопка следующего кадра:
Единожды вызывается play() 

Кнопка предыдущего кадра:
desired_frame = текущий-1
-сбрасываем видео на ноль
-запуск play() столько раз, пока не достигем desired_frame

Для перемещения по слайдеру:
Если desired_frame > текущего кадра, то вызываем play() столько раз, пока не достигнем desired_frame
Если desired_frame < текущего кадра, то сбрасываем видео на 0 и запускаем play() столько раз, пока не достигнем нужного фрейма

----------------------------------
Добавить версионность

Добавить иконку

Добавить вывод SEI на экран

Пернгнать из pro в c-make 

Почистить код от хлама

Поп-ап с ошибкой - сделать скругление
