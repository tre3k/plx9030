# plx9030

Linux driver for PCI controller plx9030

## Установка:
1. git clone https://github.com/tre3k/TEX-2/
> подгрузит весь репозиторий TEX-2 вместе с девайс серверами и драйверами
2. cd TEX-2/drivers/plx9030
> переход в папку с исходником драйвера для PCI контроллера PLX9030
3. sudo ./install.sh
> сборка и установка драйвера (необходимы исходники ядра должен быть в репозиториях дистрибутива и носит название что-то вроде linux-headers или linux-sources, скорее всего они уже установлен)
4. ./run_driver.sh start
> загрузка драйвера в ядро

После загрузки драйвера появляется символьное устройство /dev/plxdev[n], необходимое для работы plx9030 платы. Он необходим для работы мониторного счётчика (https://github.com/tre3k/TEX-2/tree/master/servers/Monitor) и двухканального контроллера вращения (https://github.com/tre3k/TEX-2/tree/master/servers/RA153.28).
