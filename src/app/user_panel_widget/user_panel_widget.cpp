#include "user_panel_widget.hpp"

#include "settings_manager.hpp"
#include "spdlog_wrapper.hpp"
#include "test_introduction_widget.hpp"
#include "test_result_widget.hpp"

#include <QLabel>
#include <QRegularExpression>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <qwidget.h>

namespace APP
{
QString createLooseRegex(const QString &command)
{
	QString core_command = command.trimmed();
	QString escaped_core = QRegularExpression::escape(core_command);

	escaped_core.replace("\\ ", "\\s+");
	escaped_core.replace(QRegularExpression::escape(":"), "\\s*:\\s*");
	escaped_core.replace(QRegularExpression::escape("="), "\\s*=\\s*");
	escaped_core.replace(QRegularExpression::escape(","), "\\s*,\\s*");
	escaped_core.replace(QRegularExpression::escape("--"), "\\s*--\\s*");

	if (core_command.startsWith("sudo "))
	{
		escaped_core.replace(QRegularExpression("^sudo\\s+"), "");
	}
	if (core_command.startsWith("nft> ") || core_command.startsWith("sudo nft> "))
	{
		escaped_core.replace(QRegularExpression("^nft>\\s+"), "");
	}

	QString final_regex = "^";
	final_regex += "\\s*";
	final_regex += "(?:sudo\\s+)?";
	final_regex += "(?:nft>\\s+)?";
	final_regex += escaped_core;
	final_regex += "\\s*";
	final_regex += "$";

	return final_regex;
}

QString createConfigRegex(const QString &param)
{
	QString escaped = QRegularExpression::escape(param);
	escaped.replace("\\s*:\\s*", "\\s*:\\s*");
	return "^\\s*" + escaped + "\\s*$";
}

QString createSimpleRegex(const QString &text)
{
	QString escaped = QRegularExpression::escape(text);
	return "^\\s*" + escaped + "\\s*$";
}

UserPanelWidget::UserPanelWidget(QWidget *parent) :
	QWidget(parent),
	m_test_generator(new TestGenerator()),
	m_current_test_index(0)
{
	this->initialize();
}

UserPanelWidget::~UserPanelWidget()
{}

void UserPanelWidget::addScreen(const ScreenInfo &screen_info)
{
	if (screen_info.type == PanelType::NONE)
	{
		spdlog::error(QObject::tr("pebkac_error_invalid_screen"));
		return;
	}

	auto index = this->m_main_layout->addWidget(screen_info.widget);

	this->m_screens.insert(index, screen_info);
}

void UserPanelWidget::switchScreen(PanelType type)
{
	int index = -1;
	for (auto it = this->m_screens.begin(); it != this->m_screens.end(); ++it)
	{
		if (it.value().type == type)
		{
			if (this->m_main_layout->currentIndex() == it.key())
			{
				return;
			}
			this->m_main_layout->setCurrentIndex(it.key());
			UTILS::SettingsManager::instance()->setDirectValue(UTILS::SettingsManager::Setting::LAST_OPEN_PANEL,
															   QVariant::fromValue(it.value().type));
			return;
		}
	}
	spdlog::error(QObject::tr("pebkac_error_invalid_screen"));
	return;
}

void UserPanelWidget::nextScreen()
{
	int index = this->m_main_layout->currentIndex();
	if (index == this->m_screens.size() - 1)
	{
		return;
	}

	this->m_main_layout->setCurrentIndex(index + 1);
	UTILS::SettingsManager::instance()->setDirectValue(UTILS::SettingsManager::Setting::LAST_OPEN_PANEL,
													   QVariant::fromValue(this->m_screens[index + 1].type));
	return;
}

void UserPanelWidget::initialize()
{
	m_test_generator->addQuestion(
		{"Какой командой можно проверить, включён ли NFQ в сборке Suricata?",
		 {"suricata -V", "suricata --list-keywords", "suricata --build-info", "iptables --version", "suricata --features"},
		 3,
		 "suricata --build-info",
		 createSimpleRegex("suricata --build-info"),
		 "Команда 'suricata --build-info' показывает информацию о сборке, включая поддержку NFQUEUE."});

	m_test_generator->addQuestion(
		{"Какое правило в iptables нужно добавить, чтобы анализировать весь трафик, проходящий через шлюз?",
		 {"sudo iptables -I INPUT -j NFQUEUE", "sudo iptables -I FORWARD -j NFQUEUE",
		  "sudo iptables -A FORWARD -p tcp -j ACCEPT", "sudo iptables -I OUTPUT -j NFQUEUE"},
		 2,
		 "sudo iptables -I FORWARD -j NFQUEUE",
		 createLooseRegex("sudo iptables -I FORWARD -j NFQUEUE"),
		 "Цепочка FORWARD используется для трафика, проходящего через машину (шлюз). 'sudo iptables -I FORWARD -j NFQUEUE' "
		 "перенаправляет этот трафик в очередь NFQ."});

	/*m_test_generator->addQuestion(
		{"Как проверить конфигурацию iptables и увидеть установленные правила со статистикой?",
		 {"suricata -T", "sudo nft list ruleset", "sudo iptables -vnL", "sudo systemctl status suricata",
		  "cat /proc/net/nf_queue"},
		 3,
		 "sudo iptables -vnL",
		 createLooseRegex("sudo iptables -vnL"),
		 "Команда 'sudo iptables -vnL' отображает текущие правила iptables с подробной статистикой (v - verbose, n - numeric, "
		 "L - list), полезной для проверки правил NFQUEUE."});*/

	/*m_test_generator->addQuestion({"Какой командой можно стереть все правила iptables?",
								   {"sudo iptables -X", "sudo iptables --flush-all", "sudo iptables -Z", "sudo iptables -F"},
								   4,
								   "sudo iptables -F",
								   createLooseRegex("sudo iptables -F"),
								   "Команда 'sudo iptables -F' удаляет (flush) все правила из всех цепочек iptables."});*/

	m_test_generator->addQuestion(
		{"Как перенаправить весь входящий TCP-трафик в очередь NFQ?",
		 {"sudo iptables -I INPUT -p tcp -j NFQUEUE", "sudo iptables -I OUTPUT -p tcp -j NFQUEUE",
		  "sudo iptables -A INPUT -j NFQUEUE", "sudo iptables -I INPUT -p udp -j NFQUEUE",
		  "sudo iptables -I INPUT -j NFQUEUE --proto tcp"},
		 1,
		 "sudo iptables -I INPUT -p tcp -j NFQUEUE",
		 createLooseRegex("sudo iptables -I INPUT -p tcp -j NFQUEUE"),
		 "Используйте 'sudo iptables -I INPUT -p tcp -j NFQUEUE' для направления входящих TCP пакетов в очередь. INPUT - "
		 "входящий трафик, -p tcp - протокол TCP."});

	m_test_generator->addQuestion(
		{"Каким правилом отправить в NFQ только исходящий HTTP-трафик на порт 80?",
		 {"sudo iptables -I INPUT -p tcp --sport 80 -j NFQUEUE", "sudo iptables -A OUTPUT -p tcp --dport 80 -j ACCEPT",
		  "sudo iptables -I OUTPUT -p tcp --dport 80 -j NFQUEUE", "sudo iptables -I OUTPUT -p udp --dport 80 -j NFQUEUE"},
		 3,
		 "sudo iptables -I OUTPUT -p tcp --dport 80 -j NFQUEUE",
		 createLooseRegex("sudo iptables -I OUTPUT -p tcp --dport 80 -j NFQUEUE"),
		 "Используйте 'sudo iptables -I OUTPUT -p tcp --dport 80 -j NFQUEUE'. OUTPUT - исходящий трафик, -p tcp - протокол "
		 "TCP, --dport 80 - порт назначения 80 (HTTP)."});

	m_test_generator->addQuestion(
		{"Как настроить iptables на локальной машине для анализа Suricata всего входящего и исходящего трафика?",
		 {"sudo iptables -A FORWARD -j NFQUEUE", "sudo iptables -I INPUT -j NFQUEUE && sudo iptables -I OUTPUT -j NFQUEUE",
		  "sudo iptables -I PREROUTING -j NFQUEUE && sudo iptables -I POSTROUTING -j NFQUEUE", "suricata -q 0 --runmode=autofp",
		  "sudo iptables -P INPUT NFQUEUE && sudo iptables -P OUTPUT NFQUEUE"},
		 2,
		 "sudo iptables -I INPUT -j NFQUEUE && sudo iptables -I OUTPUT -j NFQUEUE",
		 createLooseRegex("sudo iptables -I INPUT -j NFQUEUE && sudo iptables -I OUTPUT -j NFQUEUE"),
		 "Нужно добавить правила в цепочки INPUT (для входящего) и OUTPUT (для исходящего) трафика: 'sudo iptables -I INPUT -j "
		 "NFQUEUE && sudo iptables -I OUTPUT -j NFQUEUE'."});

	/*m_test_generator->addQuestion({"Как создать в nftables цепочку IPS с типом filter и хуком forward?",
								   {"nft add table filter", "nft create chain filter IPS", "nft add rule filter forward accept",
									"nft add chain filter IPS { type filter hook forward priority 10;}"},
								   4,
								   "nft add chain filter IPS { type filter hook forward priority 10;}",
								   createLooseRegex("nft add chain filter IPS { type filter hook forward priority 10;}"),
								   "Команда 'add chain filter IPS { type filter hook forward priority 10;}' создает цепочку "
								   "IPS в таблице filter для обработки проходящего трафика с приоритетом 10."});*/

	/*m_test_generator->addQuestion(
		{"Как в nftables отправить все пакеты из цепочки IPS в очередь Suricata?",
		 {"nft add rule filter IPS accept", "nft add rule filter IPS drop", "nft add rule filter IPS queue",
		  "nft add rule filter IPS redirect", "nft add rule filter IPS log"},
		 3,
		 "nft add rule filter IPS queue",
		 createLooseRegex("nft add rule filter IPS queue"),
		 "Действие 'queue' в правиле 'nft add rule filter IPS queue' отправляет пакеты "
		 "пользовательскому приложению (Suricata) через механизм очередей."});*/

	/*m_test_generator->addQuestion(
		{"Как в nftables ограничить отправку в очередь пакетами, идущими с eth0 на eth1?",
		 {"nft add rule filter IPS iif eth0 queue", "nft add rule filter IPS oif eth1 queue",
		  "nft add rule filter IPS iif eth0 oif eth1 queue", "nft add rule filter IPS ip saddr 192.168.1.0/24 queue"},
		 3,
		 "nft add rule filter IPS iif eth0 oif eth1 queue",
		 createLooseRegex("nft add rule filter IPS iif eth0 oif eth1 queue"),
		 "Условие 'iif eth0 oif eth1' в правиле 'nft add rule filter IPS iif eth0 oif eth1 queue' "
		 "фильтрует пакеты по входному (eth0) и выходному (eth1) интерфейсам."});*/

	/*m_test_generator->addQuestion(
		{"Как выглядит команда nftables для отправки пакетов в очереди 3-5 с балансировкой (fanout) и обходом "
		 "(bypass)?",
		 {"nft add rule filter IPS queue num 3,4,5 options fanout",
		  "nft add rule filter IPS queue num 3-5 options fanout,bypass",
		  "nft add rule filter IPS queue range 3-5 fanout bypass", "nft add rule filter IPS queue num 3-5 bypass fanout"},
		 2,
		 "nft add rule filter IPS queue num 3-5 options fanout,bypass",
		 createLooseRegex("nft add rule filter IPS queue num 3-5 options fanout,bypass"),
		 "Команда 'nft add rule filter IPS queue num 3-5 options fanout,bypass' использует диапазон очередей (num 3-5), "
		 "балансировку (fanout) и обход (bypass)."});*/

	m_test_generator->addQuestion(
		{"Как запустить Suricata для использования очередей NFQUEUE 3, 4 и 5?",
		 {"suricata -q 3-5", "suricata --queue=3,4,5", "suricata -Q 3 -Q 4 -Q 5", "suricata -q 3 -q 4 -q 5"},
		 4,
		 "suricata -q 3 -q 4 -q 5",
		 createSimpleRegex("suricata -q 3 -q 4 -q 5"),
		 "При запуске Suricata в режиме NFQ, каждая используемая очередь указывается отдельным ключом '-q'. Пример: 'suricata "
		 "-q 3 -q 4 -q 5'."});

	/*m_test_generator->addQuestion(
		{"Какой вариант в опциях 'queue' nftables включает распределение пакетов по ID процессора?",
		 {"bypass", "cpu_map", "hash", "fanout"},
		 4,
		 "fanout",
		 createSimpleRegex("fanout"),
		 "Опция 'fanout' используется в 'queue options' nftables для распределения пакетов по очередям на основе ID "
		 "процессора, обрабатывающего пакет."});

	m_test_generator->addQuestion(
		{"Какой вариант в опциях 'queue' nftables позволяет пропускать пакеты, если очередь не слушается?",
		 {"fanout", "ignore", "pass", "bypass"},
		 4,
		 "bypass",
		 createSimpleRegex("bypass"),
		 "Опция 'bypass' в правиле 'queue' nftables гарантирует, что пакеты будут пропущены ядром, если нет активного "
		 "слушателя очереди (например, Suricata остановлена)."});*/

	m_test_generator->addQuestion({"Какой параметр конфигурации AF_PACKET включает режим IPS для Suricata?",
								   {"copy-mode: tap", "stream.inline: yes", "copy-mode: ips", "use-mmap: yes", "mode: ips"},
								   3,
								   "copy-mode: ips",
								   createConfigRegex("copy-mode: ips"),
								   "Установка 'copy-mode: ips' в секции af-packet конфигурационного файла suricata.yaml "
								   "включает режим IPS (возможность блокировки пакетов) для данного интерфейса."});

	m_test_generator->addQuestion({"Какую опцию необходимо включить в AF_PACKET для использования zero-copy?",
								   {"copy-mode: tap", "defrag: no", "use-mmap: yes", "cluster-type: cluster_flow"},
								   3,
								   "use-mmap: yes",
								   createConfigRegex("use-mmap: yes"),
								   "Параметр 'use-mmap: yes' в конфигурации af-packet включает использование memory-mapped "
								   "буферов, что является основой механизма zero-copy."});

	m_test_generator->addQuestion(
		{"Что нужно указать в конфигурации Suricata для принудительного включения режима "
		 "блокировки (inline) для потоков?",
		 {"stream.inline: auto", "stream.inline: yes", "copy-mode: ips", "stream.inline: no", "inline-mode: enabled"},
		 2,
		 "stream.inline: yes",
		 createConfigRegex("stream.inline: yes"),
		 "Установка 'stream.inline: yes' в секции stream конфигурационного файла suricata.yaml "
		 "принудительно активирует inline-режим обработки потоков (если поддерживается движком)."});

	m_test_generator->addQuestion(
		{"Какое значение 'copy-mode' в AF_PACKET используется для режима “моста” (tap), где пакеты только "
		 "копируются?",
		 {"copy-mode: ips", "copy-mode: bridge", "stream.inline: no", "copy-mode: tap"},
		 4,
		 "copy-mode: tap",
		 createConfigRegex("copy-mode: tap"),
		 "Значение 'copy-mode: tap' используется в конфигурации af-packet для режима 'моста' (пассивное прослушивание), когда "
		 "Suricata только анализирует трафик без возможности блокировки."});

	m_test_generator->addQuestion(
		{"Как запустить Suricata в режиме AF_PACKET с конфигурацией из /etc/suricata/suricata.yaml?",
		 {"suricata --af-packet -i eth0", "suricata -c suricata.yaml -D", "suricata --init-errors-fatal --af-packet",
		  "suricata -c /etc/suricata/suricata.yaml --af-packet"},
		 4,
		 "suricata -c /etc/suricata/suricata.yaml --af-packet",
		 createSimpleRegex("suricata -c /etc/suricata/suricata.yaml --af-packet"),
		 "Используйте команду 'suricata -c /etc/suricata/suricata.yaml --af-packet'. Ключ '-c' указывает конфиг, '--af-packet' "
		 "включает режим AF_PACKET."});

	/*m_test_generator->addQuestion(
		{"Какое значение 'cluster-type' в AF_PACKET обеспечивает симметричный хэш по потоку (по умолчанию)?",
		 {"cluster_cpu", "cluster_flow", "cluster_round_robin", "cluster_qm", "flow_hash"},
		 2,
		 "cluster_flow",
		 createSimpleRegex("cluster_flow"),
		 "Значение 'cluster_flow' для параметра 'cluster-type' в конфигурации af-packet используется по умолчанию и "
		 "гарантирует, что пакеты одного потока попадут на один рабочий поток Suricata."});

	m_test_generator->addQuestion({"В каком файле журнала Suricata искать статистику по пакетам, обработанным ядром "
								   "(kernel_packets, kernel_drops)?",
								   {"eve.json", "suricata.log", "fast.log", "stats.log"},
								   4,
								   "stats.log",
								   createSimpleRegex("stats.log"),
								   "Файл 'stats.log' содержит периодическую статистику производительности Suricata, включая "
								   "счетчики пакетов на уровне ядра 'kernel_packets' и 'kernel_drops'."});

	m_test_generator->addQuestion(
		{"Как перенаправить в NFQ входящие TCP-пакеты с исходным портом 80?",
		 {"sudo iptables -I INPUT -p tcp --dport 80 -j NFQUEUE", "sudo iptables -I OUTPUT -p tcp --sport 80 -j NFQUEUE",
		  "sudo iptables -I INPUT -p tcp --sport 80 -j NFQUEUE", "sudo iptables -A INPUT -p tcp --port 80 -j NFQUEUE"},
		 3,
		 "sudo iptables -I INPUT -p tcp --sport 80 -j NFQUEUE",
		 createLooseRegex("sudo iptables -I INPUT -p tcp --sport 80 -j NFQUEUE"),
		 "Используйте 'sudo iptables -I INPUT -p tcp --sport 80 -j NFQUEUE'. INPUT - входящий трафик, -p tcp - протокол TCP, "
		 "--sport 80 - исходный порт 80."});*/

	m_test_generator->addQuestion(
		{"Какой командой запустить Suricata в режиме NFQ для обработки очереди 0?",
		 {"suricata --nfqueue 0", "suricata -q 0", "suricata -Q 0", "suricata --queue=0", "suricata -N 0"},
		 2,
		 "suricata -q 0",
		 createSimpleRegex("suricata -q 0"),
		 "Команда 'suricata -q 0' запускает Suricata для обработки пакетов из очереди NFQUEUE с номером 0."});

	m_test_generator->addQuestion(
		{"Какой ключ запуска Suricata активирует режим NFQ (с использованием очереди 0 по умолчанию)?",
		 {"--nfqueue", "-N", "--enable-nfq", "-q"},
		 4,
		 "-q",
		 createSimpleRegex("-q"),
		 "Ключ '-q' используется для включения режима NFQ при запуске Suricata. Если номер очереди не указан после ключа, "
		 "используется очередь 0 по умолчанию."});

	m_test_generator->addQuestion(
		{"Какая строка в конфигурации AF_PACKET для интерфейса eth1 указывает, что он "
		 "должен копировать пакеты в eth0 (режим tap/bridge)?",
		 {"copy-iface: eth0", "bridge-iface: eth0", "target-iface: eth0", "pair-iface: eth0", "output-iface: eth0"},
		 1,
		 "copy-iface: eth0",
		 createConfigRegex("copy-iface: eth0"),
		 "В секции конфигурации интерфейса eth1 для режима tap/bridge параметр 'copy-iface: eth0' "
		 "указывает, что проходящие пакеты нужно пересылать на интерфейс eth0."});

	m_test_generator->addQuestion(
		{"Какую опцию необходимо включить в конфигурации AF_PACKET для активации механизма zero-copy?",
		 {"zero-copy: enabled", "use-mmap: yes", "mmap-buffers: true", "af-packet-zerocopy: yes"},
		 2,
		 "use-mmap: yes",
		 createConfigRegex("use-mmap: yes"),
		 "Для активации zero-copy в режиме AF_PACKET установите параметр 'use-mmap: yes', который включает использование "
		 "memory-mapped буферов."});

	m_test_generator->addQuestion({"Как отключить дефрагментацию IP-пакетов для интерфейса в режиме AF_PACKET?",
								   {"defragmentation: disabled", "ip-defrag: false", "defrag: no", "reassembly: off"},
								   3,
								   "defrag: no",
								   createConfigRegex("defrag: no"),
								   "Установите 'defrag: no' в настройках интерфейса AF_PACKET в suricata.yaml, чтобы отключить "
								   "встроенный механизм дефрагментации."});

	m_test_generator->addQuestion(
		{"Какой параметр конфигурации AF_PACKET задаёт размер кольцевого буфера (например, 64535 байт)?",
		 {"ring-size: 65536", "packet-buffer: 64K", "mmap-size: 64535", "buffer-size: 64535"},
		 4,
		 "buffer-size: 64535",
		 createConfigRegex("buffer-size:\\s*\\d+"),
		 "Параметр 'buffer-size' устанавливает размер кольцевого буфера RX_RING/TX_RING в байтах, например 'buffer-size: "
		 "64535'."});

	m_test_generator->addQuestion({"Какой параметр должен иметь одинаковое значение для интерфейсов AF_PACKET, "
								   "работающих в паре (кластере), для корректной балансировки?",
								   {"threads", "cluster-id", "copy-mode", "buffer-size", "interface-group"},
								   2,
								   "cluster-id",
								   createSimpleRegex("cluster-id"),
								   "Параметр 'cluster-id' используется для группировки нескольких интерфейсов AF_PACKET в один "
								   "логический кластер. Интерфейсы с одинаковым cluster-id обрабатываются совместно."});

	m_test_generator->addQuestion(
		{"Как запустить Suricata в режиме AF_PACKET только на интерфейсе eth0, используя конфиг "
		 "/etc/suricata/suricata.yaml?",
		 {"suricata --af-packet -i eth0", "suricata -i eth0 --af-packet",
		  "suricata -c /etc/suricata/suricata.yaml --af-packet=eth0",
		  "suricata -c suricata.yaml --interface eth0 --mode af-packet"},
		 3,
		 "suricata -c /etc/suricata/suricata.yaml --af-packet=eth0",
		 createSimpleRegex("suricata -c /etc/suricata/suricata.yaml --af-packet=eth0"),
		 "Команда 'suricata -c /etc/suricata/suricata.yaml --af-packet=eth0' запускает Suricata с указанным конфигом, "
		 "активируя AF_PACKET только для интерфейса eth0, переопределяя настройки из файла."});

	/*m_test_generator->addQuestion(
		{"Как перенаправить исходящий DNS-трафик (UDP, порт назначения 53) в очередь NFQ?",
		 {"sudo iptables -I INPUT -p udp --sport 53 -j NFQUEUE", "sudo iptables -I OUTPUT -p tcp --dport 53 -j NFQUEUE",
		  "sudo iptables -A OUTPUT -p udp --port 53 -j QUEUE", "sudo iptables -I OUTPUT -p udp --dport 53 -j NFQUEUE"},
		 4,
		 "sudo iptables -I OUTPUT -p udp --dport 53 -j NFQUEUE",
		 createLooseRegex("sudo iptables -I OUTPUT -p udp --dport 53 -j NFQUEUE"),
		 "Используйте 'sudo iptables -I OUTPUT -p udp --dport 53 -j NFQUEUE'. OUTPUT - исходящий трафик, -p udp - протокол "
		 "UDP, --dport 53 - порт назначения DNS."});

	m_test_generator->addQuestion(
		{"Как в nftables настроить балансировку (fanout) по очередям 3-5 без опции bypass?",
		 {"nft add rule filter IPS queue num 3-5 options bypass", "nft add rule filter IPS queue num 3,4,5 fanout",
		  "nft add rule filter IPS queue fanout num 3-5", "nft add rule filter IPS queue num 3-5 options fanout"},
		 4,
		 "nft add rule filter IPS queue num 3-5 options fanout",
		 createLooseRegex("nft add rule filter IPS queue num 3-5 options fanout"),
		 "Команда 'nft add rule filter IPS queue num 3-5 options fanout' настроит отправку пакетов в очереди 3, 4, 5 с "
		 "балансировкой fanout, но без bypass (пакеты будут ожидать обработки)."});*/

	m_test_generator->addQuestion({"Какую опцию AF_PACKET установить в 'no', чтобы отключить zero-copy (например, "
								   "для режима IDS без этой оптимизации)?",
								   {"copy-mode: no", "use-mmap: no", "zero-copy: no", "defrag: no"},
								   2,
								   "use-mmap: no",
								   createConfigRegex("use-mmap: no"),
								   "Установка 'use-mmap: no' в конфигурации af-packet отключает zero-copy, что переводит "
								   "интерфейс в режим без этой оптимизации."});

	/*m_test_generator->addQuestion({"Какой приоритет выполнения задан для цепочки IPS в команде 'nft add chain "
								   "filter IPS { type filter hook forward priority 10;}'?",
								   {"filter", "forward", "10", "0", "default"},
								   3,
								   "10",
								   createSimpleRegex("10"),
								   "В приведенной команде 'nft add chain filter IPS { type filter hook forward priority 10;}' "
								   "параметр 'priority 10' задает приоритет выполнения этой цепочки."});

	m_test_generator->addQuestion(
		{"Какая команда iptables отправит входящий HTTP-трафик (TCP, порт назначения 80) в NFQUEUE?",
		 {"sudo iptables -I OUTPUT -p tcp --dport 80 -j NFQUEUE", "sudo iptables -I INPUT -p tcp --sport 80 -j NFQUEUE",
		  "sudo iptables -A INPUT -p tcp --dport 80 -j ACCEPT", "sudo iptables -I INPUT -p tcp --dport 80 -j NFQUEUE"},
		 4,
		 "sudo iptables -I INPUT -p tcp --dport 80 -j NFQUEUE",
		 createLooseRegex("sudo iptables -I INPUT -p tcp --dport 80 -j NFQUEUE"),
		 "Команда 'sudo iptables -I INPUT -p tcp --dport 80 -j NFQUEUE' перенаправляет входящий трафик (-I INPUT) протокола "
		 "TCP (-p tcp) на порт назначения 80 (--dport 80) в очередь NFQUEUE (-j NFQUEUE)."});

	m_test_generator->addQuestion(
		{"Какой ключ iptables используется для указания протокола (TCP, UDP, ICMP и т.д.)?",
		 {"- P", "--protocol", "-p", "-proto"},
		 3,
		 "-p",
		 createSimpleRegex("-p"),
		 "Ключ '-p' (protocol) используется в iptables для задания протокола фильтрации (например, '-p tcp', '-p udp')."});

	m_test_generator->addQuestion(
		{"Какой ключ iptables используется для фильтрации по порту назначения?",
		 {"--sport", "--dest-port", "-dp", "--dport"},
		 4,
		 "--dport",
		 createSimpleRegex("--dport"),
		 "Используйте ключ '--dport' (destination port) для фильтрации по порту назначения (например, '--dport 80')."});

	m_test_generator->addQuestion(
		{"Какой ключ iptables используется для фильтрации по порту-источнику?",
		 {"--sport", "--dport", "--source-port", "-sp", "--src-port"},
		 1,
		 "--sport",
		 createSimpleRegex("--sport"),
		 "Используйте ключ '--sport' (source port) для фильтрации по порту источника (например, '--sport 53')."});*/

	m_test_generator->addQuestion({"Какое значение 'stream.inline' позволяет Suricata автоматически включать режим "
								   "inline, если он поддерживается?",
								   {"stream.inline: yes", "stream.inline: auto", "stream.inline: no", "stream.inline: dynamic"},
								   2,
								   "stream.inline: auto",
								   createConfigRegex("stream.inline: auto"),
								   "Установка 'stream.inline: auto' позволяет Suricata автоматически активировать inline-режим "
								   "для потоков, если используемый режим захвата (NFQ, AF_PACKET IPS) его поддерживает."});

	m_test_generator->addQuestion(
		{"Какой параметр в секции af-packet задает идентификатор кластера для интерфейса или группы интерфейсов?",
		 {"cluster-type", "interface-id", "group-id", "cluster-id"},
		 4,
		 "cluster-id",
		 createSimpleRegex("cluster-id"),
		 "Параметр 'cluster-id' в конфигурации af-packet задает идентификатор кластера, позволяя объединять интерфейсы для "
		 "совместной обработки трафика."});

	/*m_test_generator->addQuestion(
		{"Как полностью отключить дефрагментацию пакетов в конфигурации AF_PACKET для интерфейса?",
		 {"defrag: false", "fragmentation: off", "defrag: no", "disable-defrag: yes"},
		 3,
		 "defrag: no",
		 createConfigRegex("defrag: no"),
		 "Параметр 'defrag: no' в настройках интерфейса AF_PACKET отключает дефрагментацию IP пакетов."});*/

	m_test_generator->addQuestion(
		{"Какой параметр конфигурации AF_PACKET определяет количество потоков для обработки трафика с интерфейса?",
		 {"workers", "cpu-affinity", "threads", "thread-count", "num-threads"},
		 3,
		 "threads",
		 createSimpleRegex("threads"),
		 "Параметр 'threads' в секции af-packet интерфейса указывает количество рабочих потоков Suricata, которые будут "
		 "обрабатывать пакеты с этого интерфейса."});

	/*m_test_generator->addQuestion(
		{"Как перенаправить входящий UDP-трафик с исходным портом 53 (ответы DNS) в NFQUEUE?",
		 {"sudo iptables -I INPUT -p udp --dport 53 -j NFQUEUE", "sudo iptables -I OUTPUT -p udp --sport 53 -j NFQUEUE",
		  "sudo iptables -A INPUT -p udp --source-port 53 -j NFQUEUE", "sudo iptables -I INPUT -p udp --sport 53 -j NFQUEUE"},
		 4,
		 "sudo iptables -I INPUT -p udp --sport 53 -j NFQUEUE",
		 createLooseRegex("sudo iptables -I INPUT -p udp --sport 53 -j NFQUEUE"),
		 "Команда 'sudo iptables -I INPUT -p udp --sport 53 -j NFQUEUE' перехватывает входящие (-I INPUT) UDP пакеты (-p udp) "
		 "с портом источника 53 (--sport 53)."});

	m_test_generator->addQuestion(
		{"Как перенаправить исходящий HTTPS-трафик (TCP, порт назначения 443) в NFQ?",
		 {"sudo iptables -I INPUT -p tcp --dport 443 -j NFQUEUE", "sudo iptables -I OUTPUT -p tcp --sport 443 -j NFQUEUE",
		  "sudo iptables -I OUTPUT -p tcp --dport 443 -j NFQUEUE", "sudo iptables -A OUTPUT -p tcp --dest-port 443 -j QUEUE"},
		 3,
		 "sudo iptables -I OUTPUT -p tcp --dport 443 -j NFQUEUE",
		 createLooseRegex("sudo iptables -I OUTPUT -p tcp --dport 443 -j NFQUEUE"),
		 "Команда 'sudo iptables -I OUTPUT -p tcp --dport 443 -j NFQUEUE' отправляет исходящий (-I OUTPUT) TCP трафик (-p tcp) "
		 "на порт 443 (--dport 443) в очередь NFQ."});

	m_test_generator->addQuestion(
		{"Какой параметр конфигурации AF_PACKET отвечает за размер кольцевого буфера?",
		 {"ring-buffer", "mmap-buffer-size", "packet-buffer", "buffer-size"},
		 4,
		 "buffer-size",
		 createSimpleRegex("buffer-size"),
		 "Параметр 'buffer-size' определяет размер кольцевого буфера (ring-buffer) в байтах для интерфейса AF_PACKET."});*/

	m_test_generator->addQuestion({"Какой режим запуска ('runmode') Suricata рекомендуется для максимальной "
								   "производительности на многоядерных системах?",
								   {"--runmode=autofp", "--runmode=workers", "--runmode=single", "--runmode=pcap"},
								   2,
								   "--runmode=workers",
								   createSimpleRegex("--runmode=workers"),
								   "Режим запуска 'workers' ('--runmode=workers') позволяет Suricata эффективно использовать "
								   "несколько потоков CPU для параллельной обработки пакетов."});

	/*m_test_generator->addQuestion({"Какой ключ iptables используется для вставки правила в начало цепочки?",
								   {"-A", "-P", "-N", "-I"},
								   4,
								   "-I",
								   createSimpleRegex("-I"),
								   "Ключ '-I' (insert) используется в iptables для вставки правила в начало (или указанную "
								   "позицию) цепочки, в отличие от '-A' (append), который добавляет в конец."});

	m_test_generator->addQuestion(
		{"Как перенаправить входящий NTP-трафик (UDP, порт назначения 123) в NFQUEUE?",
		 {"sudo iptables -I OUTPUT -p udp --dport 123 -j NFQUEUE", "sudo iptables -I INPUT -p tcp --dport 123 -j NFQUEUE",
		  "sudo iptables -A INPUT -p udp --sport 123 -j QUEUE", "sudo iptables -I INPUT -p udp --dport 123 -j NFQUEUE"},
		 4,
		 "sudo iptables -I INPUT -p udp --dport 123 -j NFQUEUE",
		 createLooseRegex("sudo iptables -I INPUT -p udp --dport 123 -j NFQUEUE"),
		 "Команда 'sudo iptables -I INPUT -p udp --dport 123 -j NFQUEUE' отправляет входящий (-I INPUT) UDP трафик (-p udp) на "
		 "порт 123 (--dport 123) в очередь NFQ."});*/

	m_test_generator->addQuestion(
		{"Какое значение 'stream.inline' принудительно включает режим блокировки (если поддерживается)?",
		 {"stream.inline: auto", "stream.inline: force", "stream.inline: aggressive", "stream.inline: yes"},
		 4,
		 "stream.inline: yes",
		 createConfigRegex("stream.inline: yes"),
		 "Установка 'stream.inline: yes' принудительно включает режим inline для обработки потоков Suricata, требуя блокировки "
		 "пакетов, если движок захвата это позволяет."});

	/*m_test_generator->addQuestion({"Какой командой iptables удалить все правила только из цепочки INPUT?",
								   {"sudo iptables -F", "sudo iptables -X INPUT", "sudo iptables -F INPUT",
									"sudo iptables -Z INPUT", "sudo iptables --flush INPUT"},
								   3,
								   "sudo iptables -F INPUT",
								   createLooseRegex("sudo iptables -F INPUT"),
								   "Для удаления правил только из конкретной цепочки (например, INPUT) используется команда "
								   "'sudo iptables -F' с указанием имени цепочки: 'sudo iptables -F INPUT'."});*/

	this->setupUi();
	this->setupStyle();
	this->setupConnections();
}

void UserPanelWidget::setupUi()
{
	this->m_main_layout = new QStackedWidget();
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(this->m_main_layout);
	this->setLayout(layout);

	ScreenInfo introduction_screen = {PanelType::TEST_INTRODUCTION, this->resolveScreenText(PanelType::TEST_INTRODUCTION),
									  this->resolveScreenWidget(PanelType::TEST_INTRODUCTION)};
	ScreenInfo test_result_screen  = {PanelType::TEST_RESULT, this->resolveScreenText(PanelType::TEST_RESULT),
									  this->resolveScreenWidget(PanelType::TEST_RESULT)};

	this->addScreen(introduction_screen);
	this->addScreen(test_result_screen);

	// switchScreen(
	//	UTILS::SettingsManager::instance()->getValue(UTILS::SettingsManager::Setting::LAST_OPEN_PANEL).value<PanelType>());

	switchScreen(PanelType::TEST_INTRODUCTION);
}

void UserPanelWidget::setupConnections()
{}

void UserPanelWidget::setupStyle()
{
	setStyleSheet("UserPanelWidget { background: #181818; }"
				  "QLabel { color: #EEEEEE; }");
}

QWidget *UserPanelWidget::resolveScreenWidget(PanelType type)
{
	switch (type)
	{
		case PanelType::TEST_INTRODUCTION: {
			IntroductionWidget *widget = new IntroductionWidget();
			connect(widget, &IntroductionWidget::onStartTestClicked, this, [this]() {
				generateQuestions();
				switchScreen(PanelType::TEST_PAGE);
				emit testStarted();
			});
			return std::move(widget);
		}
		case PanelType::TEST_RESULT: {
			ResultWidget *widget = new ResultWidget();
			connect(widget, &ResultWidget::onStartTestClicked, this, [this]() {
				cleanupQuestions();
				switchScreen(PanelType::TEST_INTRODUCTION);
			});
			return std::move(widget);
		}
		default: {
			if (type >= PanelType::TEST_PAGE)
			{
				int			test_index = static_cast<int>(type) - static_cast<int>(PanelType::TEST_PAGE);
				TestWidget *widget	   = new TestWidget(this->m_questions[test_index]);

				connect(widget, &TestWidget::taskSubmitted, this, [this, test_index, widget]() {
					auto answer = widget->getAnswer();
					m_answers.append(answer);

					if (m_questions.size() - 1 <= test_index)
					{
						switchScreen(PanelType::TEST_RESULT);
						static_cast<ResultWidget *>(this->m_main_layout->currentWidget())->setTestAnswers(m_answers);
						emit testFinished();
					}
					else
					{
						nextScreen();
					}
				});
				return std::move(widget);
			}

			return new QWidget();
		}
	}
}

QString UserPanelWidget::resolveScreenText(PanelType type) const
{
	switch (type)
	{
		default:
			return "";
	}
}

void UserPanelWidget::cleanupQuestions()
{
	const auto keys = m_screens.keys();
	for (const auto &key : keys)
	{
		const auto &screen = m_screens[key];
		if (static_cast<int>(screen.type) > static_cast<int>(PanelType::TEST_RESERVED))
		{
			m_main_layout->removeWidget(screen.widget);
			delete screen.widget;
			m_screens.remove(key);
		}
	}

	m_answers.clear();
	m_questions.clear();
}

void UserPanelWidget::generateQuestions()
{
	m_answers.clear();
	auto test_questions = UTILS::SettingsManager::instance()->getValue(UTILS::SettingsManager::Setting::TEST_QUESTIONS).toInt();
	this->m_questions	= m_test_generator->getQuestions(test_questions);

	for (int i = 0; i < this->m_questions.size(); ++i)
	{
		PanelType  test_index  = static_cast<PanelType>(static_cast<int>(PanelType::TEST_PAGE) + i);
		ScreenInfo test_screen = {test_index, this->resolveScreenText(test_index), this->resolveScreenWidget(test_index)};
		this->addScreen(test_screen);
	}
}

void UserPanelWidget::finishPrematurely()
{
	m_answers.clear();
	const auto keys = m_screens.keys();
	for (const auto &key : keys)
	{
		const auto &screen = m_screens[key];
		if (static_cast<int>(screen.type) > static_cast<int>(PanelType::TEST_RESERVED))
		{
			auto answer = static_cast<TestWidget *>(screen.widget)->getAnswer();
			m_answers.append(answer);
		}
	}
	switchScreen(PanelType::TEST_RESULT);
	static_cast<ResultWidget *>(this->m_main_layout->currentWidget())->setTestAnswers(m_answers);
	emit testFinished();
}
} // namespace APP
