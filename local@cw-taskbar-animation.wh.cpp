// ==WindhawkMod==
// @id                cw-taskbar-animation
// @name              CW-任务栏动画
// @name:zh-CN        CW-任务栏动画
// @description       macOS-style icon animations for the taskbar (updated fork with support for all taskbar positions and multi-monitor fixes), plus instant-show animations for the auto-hidden taskbar.
// @description:zh-CN 任务栏动画合集：图标悬停放大动画（支持所有任务栏位置和多显示器），加上自动隐藏任务栏的即时显示动画。
// @version           2.1.0
// @author            incconutwo (Plus fork), Ph0en1x-dev (original), Bo0ii, CW-Beautify
// @github            https://github.com/incconutwo
// @homepage          https://github.com/Bo0ii/windhawk-mods
// @include           explorer.exe
// @architecture      x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore -lwindowsapp -luser32 -lversion -ldwmapi -lkernel32
// ==/WindhawkMod==

// CW-Beautify 整合说明：本模组由「CW-任务栏 Dock 动画」与「CW-任务栏秒显」合并而成。
// - 图标动画：Taskbar Dock Animation Plus（incconutwo fork），原版 Taskbar Dock Animation
//   （Ph0en1x-dev，GPL-3.0）；CW 新增「弹跳抬升幅度」「防裁剪保护」，中文界面完善。
// - 任务栏秒显：Taskbar Auto-Hide Instant Show（Bo0ii，GPL-3.0），代码置于 TaskbarAutoHide
//   命名空间，逻辑与上游逐字节一致，仅入口改名（Wh_ModInit→Init 等）并把设置键加
//   autoHide. 前缀隔离（Windows 注册表值名不区分大小写，避免与宿主 AnimationType 等
//   图标动画设置互相覆盖）。
// 两部分入口由本模组统一接入：秒显初始化失败时仅记录日志并降级，不影响图标动画。

// ==WindhawkModReadme==
/*
# CW-任务栏动画

任务栏动画合集，包含两部分功能：

1. **图标动画**（原 CW-任务栏 Dock 动画）：macOS 风格的任务栏图标悬停动画——悬停放大、相邻图标让位、空闲「呼吸」弹跳。
2. **任务栏秒显**（原 CW-任务栏秒显）：配合系统「自动隐藏任务栏」使用，鼠标一碰屏幕边缘任务栏立即出现，出现/隐藏延迟与动画可自定义。

> 图标动画需要 Windows 11 或更高版本；任务栏秒显支持 Windows 10 与 Windows 11。

## 功能一：图标动画（悬停放大）

- 悬停放大动画，三种动画曲线：余弦（平滑）、线性、指数（更灵敏）。
- 相邻图标间距让位，灵敏度可调。
- 空闲时的「呼吸」弹跳动画，可关闭或推迟；弹跳平滑淡入淡出。
- **支持所有任务栏位置**（下/上/左/右，自动检测）。
- **多显示器稳定性修复**；设置与任务栏位置变化实时生效，无需重启。
- 可排除开始、搜索、任务视图、小组件等系统按钮。
- CW 新增：**弹跳抬升幅度（%）**（空闲「呼吸」弹跳高度的倍率，50%-300%，默认 100）与**防裁剪保护**（默认开启，自动把弹跳限制在任务栏窗口内，避免图标被上边缘平口裁切）。

## 功能二：任务栏秒显（自动隐藏即时显示）

> 需先在系统设置中开启「自动隐藏任务栏」（设置 > 个性化 > 任务栏 > 任务栏行为）。本功能只优化自动隐藏时任务栏显示/隐藏的延迟与动画，不会代替你开启自动隐藏。

- **秒显**：出现延迟默认从 Windows 的 50ms 降到 1ms，鼠标碰到屏幕边缘任务栏立即出现。
- **快藏**：隐藏延迟默认从 Windows 的 500ms 降到 1ms，鼠标离开后任务栏立即收起。
- **动画类型**：无、滑动（加速版 Windows 滑动）、弹性（弹簧过冲振荡）、回弹、淡入淡出、滑动+淡入淡出（五次缓动，最顺滑）、轻微过冲。
- **速度可控**：滑动模式按百分比加速；其余模式按时长（ms）控制；帧率可调，高刷屏可用 120。

## 设置说明

### 图标动画

- **动画曲线**：0=余弦（平滑），1=线性，2=指数（更灵敏）。
- **最大缩放（%）**：图标最大放大到多少，如 130 表示放大到 130%。
- **弹跳抬升幅度（%）**：空闲「呼吸」弹跳高度的倍率（50-300，默认 100）。
- **禁用防裁剪保护**：默认开启保护；关闭后按原始幅度弹跳。
- **效果半径（像素）**：距离光标多远范围内应用动画。
- **间距灵敏度（%）**：动画时相邻图标拉开的程度，默认 50。
- **弹跳开始延迟（毫秒）**：光标停止后等多久开始「呼吸」动画，默认 500。
- **聚焦动画时长（毫秒）**：淡入淡出时长，默认 150。
- **任务栏标签兼容模式**：任务栏显示窗口标题（矩形按钮）时开启，修正动画居中。
- **排除系统按钮**：0=全部动画，1=排除开始，2=仅应用图标动画，3=等同 0。
- **平滑度（Lerp 速度）**：越高越灵敏，越低越平滑，0 为禁用。
- **禁用垂直「弹跳」**：禁用上下「呼吸」动画。
- **禁用弹跳效果**：完全关闭空闲「呼吸」。

### 任务栏秒显（设置页「任务栏秒显」嵌套组）

- **动画类型（秒显）**：任务栏出现和消失的方式，默认「滑动」。
  - **无动画**：瞬间到位，不滑动也不淡入。
  - **滑动**（默认）：Windows 原生滑动但加速，用「显示/隐藏加速（%）」控制速度。
  - **弹性**（弹簧过冲）：任务栏冲过目标位置后像弹簧一样来回振荡。
  - **回弹**：任务栏在最终位置弹跳一下。
  - **淡入淡出**：任务栏原地淡入/淡出。
  - **滑动+淡入淡出**：滑动叠加淡入淡出（五次缓动），手感最顺滑。
  - **轻微过冲**：小幅冲过目标位置再回落。
- **显示加速 %（秒显，仅滑动模式）**：出现滑动动画的播放速度，100 = Windows 原速，200 = 2 倍，400 = 4 倍。默认 400。
- **隐藏加速 %（秒显，仅滑动模式）**：隐藏滑动动画的播放速度，同上。默认 400。
- **显示时长 ms（秒显，弹性/回弹/淡入淡出模式）**：非滑动模式出现动画的时长（毫秒），越低越快，建议 150-300。默认 200。
- **隐藏时长 ms（秒显，弹性/回弹/淡入淡出模式）**：非滑动模式隐藏动画的时长（毫秒），同上。默认 200。
- **动画帧率（秒显）**：所有动画的目标帧率，越高越顺滑；60 适合大多数屏幕，高刷屏可用 120。默认 60。
- **出现延迟 ms（秒显）**：鼠标移到屏幕边缘后任务栏开始出现前的延迟，Windows 默认 50，设为 1 即立即。默认 1。
- **隐藏延迟 ms（秒显）**：鼠标离开后任务栏开始隐藏前的延迟，Windows 默认 500，设为 1 即立即。默认 1。
- **在 Windows 11 上使用旧版任务栏（秒显）**：使用 ExplorerPatcher 等工具在 Windows 11 上恢复 Windows 10 任务栏时开启。
- **空显示器边沿检测（秒显）**：鼠标悬停屏幕边缘但任务栏不出现时开启（例如该显示器上没有可见窗口，或全屏程序吞掉了鼠标边缘事件）；会创建一个后台线程轮询光标位置。

提示：**滑动**模式用「显示/隐藏加速（%）」控制速度，其余模式用「显示/隐藏时长（ms）」控制快慢；帧率对所有动画类型生效。

## 同时使用两部分

两部分功能相互独立、可单独或同时启用：图标动画挂在 Taskbar.View.dll 的指针事件上，秒显挂在任务栏窗口的 SlideWindow 与显示/隐藏定时器上。自动隐藏的任务栏出现后，把鼠标移到任务栏上，图标悬停动画照常生效。

## 兼容性与已知问题

- 仅注入 `explorer.exe`（x86-64）。
- 图标动画：放大后的图标可能略显模糊；配合 CW-任务栏高度与图标大小做窄任务栏时，请把「最大缩放」保持在 130% 或以下，减少裁切（原作者建议）；不支持 StartAllBack（需要重写整个代码库，超出范围）。
- 任务栏秒显：必须先在系统中开启「自动隐藏任务栏」才会生效；配合 ExplorerPatcher 的旧版任务栏时，请开启「在 Windows 11 上使用旧版任务栏」；个别情况下（空显示器、全屏程序）鼠标碰屏幕边缘任务栏可能不出现，可尝试开启「空显示器边沿检测」。

## 更新日志

- **2.1.0（CW）**：「CW-任务栏秒显」（Bo0ii，Taskbar Auto-Hide Instant Show）并入本模组，合并版更名 CW-任务栏动画。秒显代码置于 TaskbarAutoHide 命名空间，设置项迁移为 autoHide.* 前缀（设置页「任务栏秒显」嵌套组）；秒显初始化失败时仅降级该功能，不影响图标动画。
- **2.0.2（CW）**：基于增强版 2.0.1 换底座；新增「弹跳抬升幅度」与「防裁剪保护」；中文界面完善。
- **2.0.1（Plus，incconutwo）**：支持所有任务栏位置；多显示器稳定性修复；呼吸动画平滑淡入淡出；设置实时更新。
- **1.9.3-1.9.4（CW）**：中文汉化；新增弹跳抬升幅度与防裁剪保护（旧底座）。
- **2.3（秒显）**：汉化元数据、说明与设置文案，代码与上游 2.2 一致（并入前的最后独立版本）。

## 致谢与许可

- 图标动画原版 mod：Taskbar Dock Animation，作者 [Ph0en1x-dev](https://github.com/Ph0en1x-dev)，GPL-3.0，链接：<https://windhawk.net/mods/taskbar-dock-animation>。
- 图标动画增强版 fork：Taskbar Dock Animation Plus，作者 incconutwo，链接：<https://windhawk.net/mods/taskbar-dock-animation-plus>。
- 任务栏秒显原版 mod：Taskbar Auto-Hide Instant Show，作者 [Bo0ii](https://github.com/Bo0ii)，GPL-3.0，原版链接：<https://windhawk.net/mods/taskbar-autohide-instant-show>。
- 本合并版由 CW-Beautify 整合包（作者 Heptalune）完成：命名空间隔离、设置键 autoHide.* 前缀迁移与入口统一接入；同样以 GPL-3.0 发布。
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- AnimationType: 0
  $name: Animation Curve
  $name:uk-UA: Крива анімації
  $name:zh-CN: 动画曲线
  $name:ja-JP: アニメーション曲線
  $name:ko-KR: 애니메이션 곡선
  $name:pt-BR: Curva de animação
  $name:it-IT: Curva di animazione
  $description: Choose the feel of the scaling (0 = Cosine (Smooth), 1 = Linear, 2 = Exponential (Snappy)
  $description:uk-UA: Оберіть тип анімації (0 = Косинус (Плавно), 1 = Лінійна, 2 = Експоненціальна (Швидко)
  $description:zh-CN: 选择缩放的动画感觉（0=余弦（平滑），1=线性，2=指数（更灵敏））
  $description:ja-JP: 拡大縮小の動き方を選択（0=コサイン（滑らか）、1=線形、2=指数（キビキビ））
  $description:ko-KR: 확대/축소 애니메이션 느낌 선택 (0=코사인(부드러움), 1=선형, 2=지수(빠릿함))
  $description:pt-BR: Escolha o tipo de escala (0 = Cosseno (suave), 1 = Linear, 2 = Exponencial (rápido))
  $description:it-IT: Scegli il tipo di scala (0 = Coseno (fluida), 1 = Lineare, 2 = Esponenziale (reattiva))
- MaxScale: 130
  $name: Maximum scale (%)
  $name:uk-UA: Максимальний розмір (%)
  $name:zh-CN: 最大缩放（%）
  $name:ja-JP: 最大倍率（%）
  $name:ko-KR: 최대 확대 비율(%)
  $name:pt-BR: Escala máxima (%)
  $name:it-IT: Scala massima (%)
  $description: How large an icon can grow (e.g., 160 = +60%)
  $description:uk-UA: Наскільки сильно іконка може збільшитись (напр., 160 = +60%)
  $description:zh-CN: 图标最大可放大到多少（例如 160 = 增加 60%）
  $description:ja-JP: "アイコンの最大拡大率（例: 160 = +60%）"
  $description:ko-KR: "아이콘이 커질 수 있는 최대 크기 (예: 160 = +60%)"
  $description:pt-BR: "Até quanto o ícone pode aumentar (ex.: 160 = +60%)"
  $description:it-IT: "Quanto può ingrandirsi un’icona (es.: 160 = +60%)"
- BounceAmplitude: 100
  $name: Bounce amplitude (%)
  $name:zh-CN: 弹跳抬升幅度（%）
  $description: Multiplier for the idle breathing bounce height (100 = default, 200 = twice as high).
  $description:zh-CN: 空闲“呼吸”弹跳高度的倍率（100=默认，200=两倍高度）。防裁剪保护开启时，超出任务栏上边缘的部分会被自动收住。
- DisableAntiClipGuard: false
  $name: Disable anti-clip guard
  $name:zh-CN: 禁用防裁剪保护
  $description: Automatically limits the bounce so icons are not flat-cut by the taskbar top edge.
  $description:zh-CN: 自动把弹跳限制在任务栏窗口内，避免图标被上边缘平口裁切。默认开启；关闭后按原始幅度弹跳。
- EffectRadius: 100
  $name: Effect radius (px)
  $name:uk-UA: Радіус ефекту (пкс)
  $name:zh-CN: 效果半径（像素）
  $name:ja-JP: 効果半径（px）
  $name:ko-KR: 효과 반경(px)
  $name:pt-BR: Raio do efeito (px)
  $name:it-IT: Raggio dell’effetto (px)
  $description: Distance from the cursor where the animation is applied
  $description:uk-UA: Дистанція від курсора, на якій застосовується анімація
  $description:zh-CN: 距离光标多远范围内应用动画
  $description:ja-JP: カーソルからの距離に応じてアニメーションを適用します
  $description:ko-KR: 커서로부터 애니메이션이 적용되는 거리
  $description:pt-BR: Distância do cursor em que a animação é aplicada
  $description:it-IT: Distanza dal cursore entro cui si applica l’animazione
- SpacingFactor: 50
  $name: Spacing sensitivity (%)
  $name:uk-UA: Чутливість до проміжку (%)
  $name:zh-CN: 间距灵敏度（%）
  $name:ja-JP: 間隔の感度（%）
  $name:ko-KR: 간격 민감도(%)
  $name:pt-BR: Sensibilidade do espaçamento (%)
  $name:it-IT: Sensibilità spaziatura (%)
  $description: How much neighboring icons move apart during animation (default 50%)
  $description:uk-UA: Наскільки сусідні іконки розсуваються під час анімації (за замовчуванням 50%)
  $description:zh-CN: 动画时相邻图标拉开的程度（默认 50%）
  $description:ja-JP: アニメーション中に隣のアイコンが離れる度合い（既定 50%）
  $description:ko-KR: 애니메이션 중 인접 아이콘이 벌어지는 정도(기본 50%)
  $description:pt-BR: Quanto os ícones vizinhos se afastam durante a animação (padrão 50%)
  $description:it-IT: Quanto si allontanano le icone vicine durante l’animazione (predef. 50%)
- BounceDelay: 500
  $name: Bounce start delay (ms)
  $name:uk-UA: Затримка початку "дихання" (мс)
  $name:zh-CN: 弹跳开始延迟（毫秒）
  $name:ja-JP: バウンス開始遅延（ms）
  $name:ko-KR: 바운스 시작 지연(ms)
  $name:pt-BR: Atraso para iniciar o bounce (ms)
  $name:it-IT: Ritardo di inizio bounce (ms)
  $description: How long to wait after the cursor stops before starting the idle "breathing" animation (default 500ms)
  $description:uk-UA: Скільки чекати після зупинки курсора перед початком анімації "дихання" (за замовчуванням 500мс)
  $description:zh-CN: 光标停止后等待多久开始“呼吸”动画（默认 500ms）
  $description:ja-JP: カーソル停止後、待機時の「呼吸」アニメを開始するまでの遅延（既定 500ms）
  $description:ko-KR: 커서가 멈춘 뒤 ‘호흡’ 애니메이션을 시작하기까지 대기 시간(기본 500ms)
  $description:pt-BR: Quanto esperar após o cursor parar antes de iniciar a animação “respirar” (padrão 500ms)
  $description:it-IT: Quanto attendere dopo che il cursore si ferma prima di avviare l’animazione “respiro” (predef. 500ms)
- FocusDuration: 150
  $name: Focus animation duration (ms)
  $name:uk-UA: Тривалість анімації фокусу (мс)
  $name:zh-CN: 聚焦动画时长（毫秒）
  $name:ja-JP: フォーカスアニメ時間（ms）
  $name:ko-KR: 포커스 애니메이션 지속 시간(ms)
  $name:pt-BR: Duração da animação de foco (ms)
  $name:it-IT: Durata animazione focus (ms)
  $description: Duration of the fade-in and fade-out animation (default 150ms)
  $description:uk-UA: Тривалість анімації плавного з'явлення та згасання (за замовчуванням 150мс)
  $description:zh-CN: 淡入/淡出动画时长（默认 150ms）
  $description:ja-JP: フェードイン/アウトの時間（既定 150ms）
  $description:ko-KR: 페이드 인/아웃 애니메이션 지속 시간(기본 150ms)
  $description:pt-BR: Duração do fade-in e fade-out (padrão 150ms)
  $description:it-IT: Durata della dissolvenza in entrata/uscita (predef. 150ms)
- DisableVerticalBounce: false
  $name: Disable vertical "bounce" animation
  $name:uk-UA: Вимкнути вертикальне "дихання"
  $name:zh-CN: 禁用垂直“弹跳”动画
  $name:ja-JP: 垂直バウンスを無効化
  $name:ko-KR: 수직 바운스 애니메이션 비활성화
  $name:pt-BR: Desativar bounce vertical
  $name:it-IT: Disattiva bounce verticale
  $description: Disables the up/down "breathing" animation. Useful for vertical taskbars.
  $description:uk-UA: Вимикає анімацію "дихання" вгору/вниз. Корисно для вертикальних панелей.
  $description:zh-CN: 禁用上下“呼吸”动画。对垂直任务栏有用。
  $description:ja-JP: 上下の「呼吸」アニメを無効化します。縦型タスクバーに便利です。
  $description:ko-KR: 위/아래 ‘호흡’ 애니메이션을 비활성화합니다. 세로 작업 표시줄에 유용합니다.
  $description:pt-BR: Desativa a animação de “respirar” para cima/baixo. Útil para barras verticais.
  $description:it-IT: Disattiva l’animazione “respiro” su/giù. Utile per barre verticali.
- TaskbarLabelsMode: false
  $name: Taskbar Labels Compatibility
  $name:uk-UA: Сумісність із підписами (Labels)
  $name:zh-CN: 任务栏标签兼容模式
  $name:ja-JP: タスクバーラベル互換
  $name:ko-KR: 작업 표시줄 라벨 호환
  $name:pt-BR: Compatibilidade com rótulos
  $name:it-IT: Compatibilità etichette
  $description: Enable this if your taskbar shows window titles (rectangular icons) to fix animation centering.
  $description:uk-UA: Увімкніть, якщо ваша панель завдань відображає назви вікон (прямокутні кнопки), щоб виправити центрування.
  $description:zh-CN: 若任务栏显示窗口标题（矩形按钮），启用此项以修正动画居中。
  $description:ja-JP: タスクバーにウィンドウ名（矩形ボタン）が表示される場合、中央揃えを修正するために有効化します。
  $description:ko-KR: 작업 표시줄에 창 제목(직사각형 버튼)이 표시될 때 애니메이션 중앙 정렬을 수정합니다.
  $description:pt-BR: Ative se a barra mostrar títulos de janelas (ícones retangulares) para corrigir o alinhamento.
  $description:it-IT: Abilita se la barra mostra i titoli delle finestre (pulsanti rettangolari) per correggere la centratura.
- ExcludeSystemButtonsMode: 0
  $name: Exclude system buttons
  $name:uk-UA: Ігнорувати системні кнопки
  $name:zh-CN: 排除系统按钮
  $name:ja-JP: システムボタンを除外
  $name:ko-KR: 시스템 버튼 제외
  $name:pt-BR: Excluir botões do sistema
  $name:it-IT: Escludi pulsanti di sistema
  $description: 0=Animate all, 1=Exclude Start, 2=Animate apps icons only
  $description:uk-UA: 0=Анімація всього, 1=Без анімації Start, 2=Анімація тільки іконок застосунків
  $description:zh-CN: 0=全部动画，1=排除开始，2=仅应用图标动画
  $description:ja-JP: 0=すべてアニメ、1=スタート除外、2=アプリのみ
  $description:ko-KR: 0=전체, 1=시작 제외, 2=앱만
  $description:pt-BR: 0=Animar tudo, 1=Excluir Iniciar, 2=Somente apps
  $description:it-IT: 0=Anima tutto, 1=Escludi Start, 2=Solo app
- LerpSpeed: 60
  $name: Smoothing (Lerp speed)
  $name:uk-UA: Плавність (швидкість Lerp)
  $name:zh-CN: 平滑度（Lerp 速度）
  $name:ja-JP: スムージング（Lerp 速度）
  $name:ko-KR: 부드러움(Lerp 속도)
  $name:pt-BR: Suavização (velocidade do Lerp)
  $name:it-IT: Smussamento (velocità Lerp)
  $description: Higher = snappier, lower = smoother. 0 = disabled (direct)
  $description:uk-UA: Більше = швидше, менше = плавніше. 0 = вимкнути (напряму)
  $description:zh-CN: 越高越灵敏，越低越平滑。0=禁用（直接应用）
  $description:ja-JP: 大きいほどキビキビ、小さいほど滑らか。0=無効（直接適用）
  $description:ko-KR: 높을수록 빠릿함, 낮을수록 부드러움. 0=비활성화(직접)
  $description:pt-BR: Maior = mais rápido, menor = mais suave. 0 = desativado (direto)
  $description:it-IT: Più alto = più reattivo, più basso = più fluido. 0 = disattivato (diretto)
- DisableBounce: false
  $name: Disable bounce effect
  $name:uk-UA: Вимкнути bounce ефект
  $name:zh-CN: 禁用弹跳效果
  $name:ja-JP: バウンス効果を無効化
  $name:ko-KR: 바운스 효과 비활성화
  $name:pt-BR: Desativar efeito bounce
  $name:it-IT: Disattiva effetto bounce
  $description: Disables the idle "breathing" bounce entirely
  $description:uk-UA: Повністю вимикає bounce ("дихання")
  $description:zh-CN: 完全禁用空闲时的“呼吸”弹跳
  $description:ja-JP: 待機時の「呼吸」バウンスを完全に無効化します
  $description:ko-KR: 대기 상태 ‘호흡’ 바운스를 완전히 비활성화합니다
  $description:pt-BR: Desativa totalmente o bounce ocioso (“respirar”)
  $description:it-IT: Disattiva completamente il bounce inattivo (“respiro”)
- autoHide:
  - animationType: slide
    $name: Animation type (auto-hide)
    $name:zh-CN: 动画类型（秒显）
    $description: How the taskbar appears and disappears
    $description:zh-CN: 任务栏出现和消失的方式（秒显动画）
    $options:
      - none: 无动画（立即显示）
      - slide: 滑动（默认，可加速）
      - elastic: 弹性（弹簧过冲）
      - bounce: 回弹
      - fade: 淡入淡出（透明度）
      - slideFade: 滑动+淡入淡出（最顺滑）
      - overshoot: 轻微过冲（小幅回落）
  - showSpeedup: 400
    $name: Show speedup % (auto-hide, Slide mode only)
    $name:zh-CN: 显示加速 %（秒显，仅滑动模式）
    $description: >-
      How fast the show slide animation plays. 100 = normal Windows speed,
      200 = 2x faster, 400 = 4x faster. Only used for Slide animation type.
    $description:zh-CN: 出现滑动动画的播放速度。100 = Windows 原速，200 = 2 倍速，400 = 4 倍速。仅对「滑动」动画类型生效。
  - hideSpeedup: 400
    $name: Hide speedup % (auto-hide, Slide mode only)
    $name:zh-CN: 隐藏加速 %（秒显，仅滑动模式）
    $description: >-
      How fast the hide slide animation plays. 100 = normal Windows speed,
      200 = 2x faster, 400 = 4x faster. Only used for Slide animation type.
    $description:zh-CN: 隐藏滑动动画的播放速度。100 = Windows 原速，200 = 2 倍速，400 = 4 倍速。仅对「滑动」动画类型生效。
  - showDuration: 200
    $name: Show duration in ms (auto-hide, Elastic/Bounce/Fade modes)
    $name:zh-CN: 显示时长 ms（秒显，弹性/回弹/淡入淡出模式）
    $description: >-
      Duration of the show animation in milliseconds for non-Slide animation
      types. Lower = faster. Try 150-300.
    $description:zh-CN: 非滑动动画类型中，出现动画的时长（毫秒）。越低越快，建议 150-300。
  - hideDuration: 200
    $name: Hide duration in ms (auto-hide, Elastic/Bounce/Fade modes)
    $name:zh-CN: 隐藏时长 ms（秒显，弹性/回弹/淡入淡出模式）
    $description: >-
      Duration of the hide animation in milliseconds for non-Slide animation
      types. Lower = faster. Try 150-300.
    $description:zh-CN: 非滑动动画类型中，隐藏动画的时长（毫秒）。越低越快，建议 150-300。
  - frameRate: 60
    $name: Animation frame rate (auto-hide)
    $name:zh-CN: 动画帧率（秒显）
    $description: >-
      Target frame rate for all animations. Higher = smoother.
      60 is good for most displays, 120 for high refresh rate monitors.
    $description:zh-CN: 所有动画的目标帧率，越高越顺滑。60 适合大多数屏幕，120 适合高刷新率显示器。
  - unhideDelay: 1
    $name: Unhide delay in ms (auto-hide)
    $name:zh-CN: 出现延迟 ms（秒显）
    $description: >-
      Delay before the taskbar starts appearing when you move the mouse to
      the screen edge. Default Windows value is 50. Set to 1 for instant.
    $description:zh-CN: 鼠标移到屏幕边缘后，任务栏开始出现前的延迟。Windows 默认为 50，设为 1 即立即出现。
  - hideDelay: 1
    $name: Hide delay in ms (auto-hide)
    $name:zh-CN: 隐藏延迟 ms（秒显）
    $description: >-
      Delay before the taskbar starts hiding after the mouse leaves.
      Default Windows value is 500. Set to 1 for instant.
    $description:zh-CN: 鼠标离开后，任务栏开始隐藏前的延迟。Windows 默认为 500，设为 1 即立即隐藏。
  - oldTaskbarOnWin11: false
    $name: Use old taskbar on Windows 11 (auto-hide)
    $name:zh-CN: 在 Windows 11 上使用旧版任务栏（秒显）
    $description: >-
      Enable this if you use ExplorerPatcher or similar to use the old
      Windows 10 taskbar on Windows 11.
    $description:zh-CN: 如果使用 ExplorerPatcher 等工具在 Windows 11 上使用旧版（Windows 10）任务栏，请开启此项。
  - edgeDetection: false
    $name: Edge detection for empty monitors (auto-hide)
    $name:zh-CN: 空显示器边沿检测（秒显）
    $description: >-
      Enable this if the taskbar does not appear when hovering the screen
      edge on a monitor with no visible windows, or when a fullscreen app
      swallows the mouse-edge event. Creates a background thread that polls
      the cursor position.
    $description:zh-CN: 鼠标悬停屏幕边缘但任务栏不出现时开启，例如该显示器上没有可见窗口，或全屏程序吞掉了鼠标边缘事件。开启后会创建一个后台线程轮询光标位置。
  $name: Taskbar instant show (auto-hide)
  $name:zh-CN: 任务栏秒显（自动隐藏即时显示）
  $description: >-
    Removes the delay before the auto-hidden taskbar appears and customizes
    its show animation.
  $description:zh-CN: 去掉自动隐藏任务栏的显示延迟并自定义出现动画。
*/
// ==/WindhawkModSettings==


#include <windhawk_utils.h>
#undef GetCurrentTime
#include <windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Input.h>
#include <vector>
#include <atomic>
#include <functional>
#include <cmath>
#include <map>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <dwmapi.h>
#include <math.h>
#include <psapi.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Automation;

// Settings container
struct {
    int animationType;
    double maxScale;
    int effectRadius;
    double spacingFactor;
    int bounceDelay;
    double focusDuration;
    bool disableVerticalBounce;
    bool taskbarLabelsMode;
    int excludeSystemButtonsMode;
    double bounceAmplitude;
    bool antiClipDisabled;
    double lerpSpeed;
    bool disableBounce;
} g_settings;


// Global flags
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_hooksApplied = false;

// Animation Loop Globals
winrt::event_token g_renderingToken;
std::atomic<bool> g_isRenderingHooked = false;
std::atomic<void*> g_activeContextKey = nullptr;
std::chrono::steady_clock::time_point g_lastSignificantMoveTime;
const double MOUSE_STOP_THRESHOLD = 0.5;

// Bounce Animation Globals
std::atomic<double> g_lastMouseX = -1.0;
std::atomic<bool> g_isBouncing = false;
std::atomic<double> g_bounceIntensity = 0.0;
std::chrono::steady_clock::time_point g_bounceStartTime;
const double BOUNCE_PERIOD_MS = 1200.0;
const double BOUNCE_SCALE_AMOUNT = 0.05;
const double BOUNCE_TRANSLATE_Y = -4.0;

// Focus/Fade Animation Globals
std::atomic<bool> g_isMouseInside = false;
std::atomic<double> g_animationIntensity = 0.0;
std::chrono::steady_clock::time_point g_lastRenderTime;

enum class TaskbarEdge {
    Bottom,
    Top,
    Left,
    Right
};

struct IconAnimState {
    double waveScale = 1.0;
    double waveTranslateX = 0.0;
};

// Per-monitor context
struct TaskbarIconInfo {
    winrt::weak_ref<FrameworkElement> element;
    double originalCenterX = 0.0;
    double elementWidth = 0.0;
    IconAnimState state;
};

struct HostSignature {
    int count = -1;
    double width = -1.0;
    uint64_t orderHash = 0;
};

struct DockAnimationContext {
    bool isInitialized = false;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    winrt::weak_ref<FrameworkElement> iconHost;
    std::vector<TaskbarIconInfo> icons;

    HostSignature lastSig;
    std::chrono::steady_clock::time_point lastDirtyCheck{};

    TaskbarEdge edge = TaskbarEdge::Bottom;
    bool isVertical = false;
    HWND hWnd = nullptr;
    RECT lastTrayRect = {0, 0, 0, 0};
};

// One context per monitor (keyed by XAML instance pointer)
std::map<void*, DockAnimationContext> g_contexts;

// Prototypes
void LoadSettings();
void ApplyAnimation(double mouseX, DockAnimationContext& ctx, double intensity, double dtSec);
void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame);
void OnTaskbarPointerMoved(void* pThis_key, Input::PointerRoutedEventArgs const& args);
void OnTaskbarPointerExited(void* pThis_key);
void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons);
void RefreshIconPositions(DockAnimationContext& ctx);
void OnCompositionTargetRendering(winrt::Windows::Foundation::IInspectable const&,
                                  winrt::Windows::Foundation::IInspectable const&);

HMODULE GetTaskbarViewModuleHandle();
bool HookTaskbarViewDllSymbols(HMODULE module);
static bool RebaseIconGeometryFast(DockAnimationContext& ctx);

HWND GetCurrentThreadTrayWindow() {
    DWORD currentThreadId = GetCurrentThreadId();
    HWND hPrimary = FindWindow(L"Shell_TrayWnd", NULL);
    if (hPrimary && GetWindowThreadProcessId(hPrimary, nullptr) == currentThreadId) {
        return hPrimary;
    }
    HWND hSec = nullptr;
    while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != nullptr) {
        if (GetWindowThreadProcessId(hSec, nullptr) == currentThreadId) {
            return hSec;
        }
    }
    POINT pt;
    if (GetCursorPos(&pt)) {
        HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (hMonitor) {
            if (hPrimary && MonitorFromWindow(hPrimary, MONITOR_DEFAULTTONULL) == hMonitor) {
                return hPrimary;
            }
            hSec = nullptr;
            while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != nullptr) {
                if (MonitorFromWindow(hSec, MONITOR_DEFAULTTONULL) == hMonitor) {
                    return hSec;
                }
            }
        }
    }
    return hPrimary;
}

void UpdateTaskbarEdge(DockAnimationContext& ctx) {
    auto frame = ctx.taskbarFrame.get();
    if (!frame) return;

    HWND hWnd = ctx.hWnd;
    if (!hWnd || !IsWindow(hWnd)) {
        hWnd = GetCurrentThreadTrayWindow();
        ctx.hWnd = hWnd;
    }

    if (!hWnd) {
        if (frame.ActualWidth() < frame.ActualHeight()) {
            ctx.edge = TaskbarEdge::Left;
        } else {
            ctx.edge = TaskbarEdge::Bottom;
        }
        ctx.isVertical = (ctx.edge == TaskbarEdge::Left || ctx.edge == TaskbarEdge::Right);
        return;
    }

    RECT rect = {0};
    if (GetWindowRect(hWnd, &rect)) {
        if (rect.left == ctx.lastTrayRect.left &&
            rect.top == ctx.lastTrayRect.top &&
            rect.right == ctx.lastTrayRect.right &&
            rect.bottom == ctx.lastTrayRect.bottom) {
            return;
        }

        ctx.lastTrayRect = rect;

        LONG w = rect.right - rect.left;
        LONG h = rect.bottom - rect.top;
        if (w < h) {
            HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfo(hMonitor, &mi)) {
                if (rect.left <= mi.rcMonitor.left + (mi.rcMonitor.right - mi.rcMonitor.left) / 2) {
                    ctx.edge = TaskbarEdge::Left;
                } else {
                    ctx.edge = TaskbarEdge::Right;
                }
            } else {
                ctx.edge = TaskbarEdge::Left;
            }
        } else {
            HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfo(hMonitor, &mi)) {
                if (rect.top <= mi.rcMonitor.top + (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2) {
                    ctx.edge = TaskbarEdge::Top;
                } else {
                    ctx.edge = TaskbarEdge::Bottom;
                }
            } else {
                ctx.edge = TaskbarEdge::Bottom;
            }
        }
        ctx.isVertical = (ctx.edge == TaskbarEdge::Left || ctx.edge == TaskbarEdge::Right);
    }
}

// Simple visual tree helpers
FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (enumCallback(child)) return child;
    }
    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                          PCWSTR className) {
    if (!element) return nullptr;
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// Animation math (cosine falloff)
double CalculateScale(double distance, double radius, double maxScale) {
    if (distance > radius) return 1.0;
    
    double t = distance / radius;
    double factor = 0.0;
    
    switch (g_settings.animationType) {
        case 1: // Linear
            factor = 1.0 - t;
            break;
        case 2: // Exponential
            factor = pow(1.0 - t, 3);
            break;
        case 0: // Cosine
        default:
            factor = (cos(t * 3.14159) + 1.0) / 2.0;
            break;
    }
    if (factor < 0) factor = 0;
    return (maxScale - 1.0) * factor + 1.0;
}

void ApplyAnimation(double mouseX, DockAnimationContext& ctx, double intensity, double dtSec) {
    const double spacingFactor = g_settings.spacingFactor;
    std::vector<double> scales(ctx.icons.size());
    std::vector<double> extraWidths(ctx.icons.size());
    double totalExpansion = 0.0;
    size_t closestIconIndex = (size_t)-1;
    double minDistance = g_settings.effectRadius + 1.0;
    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;
    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) continue;
        double distance = 0.0;
        if (g_settings.taskbarLabelsMode) {
            double iconStart = ctx.icons[i].originalCenterX; 
            double iconEnd = iconStart + ctx.icons[i].elementWidth; 
            if (mouseX < iconStart) {
                distance = iconStart - mouseX; 
            } else if (mouseX > iconEnd) {
                distance = mouseX - iconEnd; 
            } else {
                distance = 0.0; 
            }
        } else {
            distance = std::abs(mouseX - ctx.icons[i].originalCenterX);
        }
        scales[i] = CalculateScale(distance, (double)g_settings.effectRadius, g_settings.maxScale);
        double size = ctx.isVertical ? element.ActualHeight() : element.ActualWidth();
        extraWidths[i] = (scales[i] - 1.0) * size * spacingFactor;
        totalExpansion += extraWidths[i];

        if (distance < minDistance) {
            minDistance = distance;
            closestIconIndex = i;
        }
    }

    double bounceScaleFactor = 1.0;
    double bounceTranslateOffset = 0.0;

    double currentBounceIntensity = g_bounceIntensity.load();

    if (currentBounceIntensity > 0.0 && closestIconIndex != (size_t)-1 &&
        minDistance < (g_settings.effectRadius / 2.0)) {       
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - g_bounceStartTime)
                            .count();
        double t = std::fmod(elapsed_ms, BOUNCE_PERIOD_MS) / BOUNCE_PERIOD_MS;
        double normalizedWave = std::sin(t * 3.14159);
        bounceScaleFactor = 1.0 + (normalizedWave * BOUNCE_SCALE_AMOUNT *
                                g_settings.bounceAmplitude * currentBounceIntensity);
        bounceTranslateOffset = normalizedWave * BOUNCE_TRANSLATE_Y *
                                g_settings.bounceAmplitude * currentBounceIntensity;       
        
        if (ctx.isVertical) {
            if (ctx.edge == TaskbarEdge::Left) {
                bounceTranslateOffset *= -1.0;
            }
        } else {
            if (ctx.edge == TaskbarEdge::Top) {
                bounceTranslateOffset *= -1.0;
            }
        }
    }

    double alpha = 1.0;
    if (g_settings.lerpSpeed > 0.0) {
        alpha = 1.0 - std::exp(-g_settings.lerpSpeed * dtSec);
    }
    alpha = std::clamp(alpha, 0.0, 1.0);

    double cumulativeShift = 0.0;

    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) continue;
        auto tg = element.RenderTransform().try_as<TransformGroup>();
        if (!tg || tg.Children().Size() < 4) continue;
        auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
        auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
        auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
        auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

        if (!waveScale || !waveTranslate || !bounceScale || !bounceTranslate) continue;
        double selfShift = extraWidths[i] / 2.0;
        double centerOffset = totalExpansion / 2.0;
        double finalShift = cumulativeShift + selfShift - centerOffset;
        cumulativeShift += extraWidths[i];
        double targetScale = 1.0 + (scales[i] - 1.0) * intensity;
        double targetTranslate = finalShift * intensity;

        if (g_settings.lerpSpeed > 0.0) {
            ctx.icons[i].state.waveScale += (targetScale - ctx.icons[i].state.waveScale) * alpha;
            ctx.icons[i].state.waveTranslateX += (targetTranslate - ctx.icons[i].state.waveTranslateX) * alpha;
        } else {
            ctx.icons[i].state.waveScale = targetScale;
            ctx.icons[i].state.waveTranslateX = targetTranslate;
        }

        waveScale.ScaleX(ctx.icons[i].state.waveScale);
        waveScale.ScaleY(ctx.icons[i].state.waveScale);

        if (ctx.isVertical) {
            waveTranslate.X(0.0);
            waveTranslate.Y(ctx.icons[i].state.waveTranslateX);
        } else {
            waveTranslate.X(ctx.icons[i].state.waveTranslateX);
            waveTranslate.Y(0.0);
        }

        if (i == closestIconIndex && currentBounceIntensity > 0.0) {
            // CW: 防裁剪保护——把弹跳限制在任务栏窗口内（水平任务栏）。
            if (!g_settings.antiClipDisabled && !ctx.isVertical) {
                double elHeight = element.ActualHeight();
                auto frameElem = ctx.taskbarFrame.get();
                if (frameElem && elHeight > 1.0) {
                    try {
                        auto pt = element.TransformToVisual(frameElem)
                                      .TransformPoint({0, 0});
                        double topGap = pt.Y;
                        double frameH = frameElem.ActualHeight();
                        if (ctx.edge == TaskbarEdge::Top && frameH > 0) {
                            topGap = frameH - (pt.Y + elHeight);
                        }
                        double headroom = topGap - 1.0;
                        if (headroom < 0.0) {
                            headroom = 0.0;
                        }
                        double combined =
                            ctx.icons[i].state.waveScale * bounceScaleFactor;
                        double rise = (combined - 1.0) * elHeight;
                        if (rise > headroom) {
                            double maxCombined = 1.0 + headroom / elHeight;
                            double maxBounce =
                                maxCombined / ctx.icons[i].state.waveScale;
                            if (maxBounce < 1.0) {
                                maxBounce = 1.0;
                            }
                            bounceScaleFactor = maxBounce;
                            rise = (ctx.icons[i].state.waveScale *
                                        bounceScaleFactor -
                                    1.0) *
                                   elHeight;
                        }
                        double allowedUp = headroom - rise;
                        if (allowedUp < 0.0) {
                            allowedUp = 0.0;
                        }
                        if (ctx.edge == TaskbarEdge::Top) {
                            if (bounceTranslateOffset > allowedUp) {
                                bounceTranslateOffset = allowedUp;
                            }
                        } else {
                            if (bounceTranslateOffset < -allowedUp) {
                                bounceTranslateOffset = -allowedUp;
                            }
                        }
                    } catch (...) {
                    }
                }
            }
            bounceScale.ScaleX(bounceScaleFactor);
            bounceScale.ScaleY(bounceScaleFactor);

            if (!g_settings.disableBounce && !g_settings.disableVerticalBounce) {
                if (ctx.isVertical) {
                    bounceTranslate.X(bounceTranslateOffset);
                    bounceTranslate.Y(0.0);
                } else {
                    bounceTranslate.X(0.0);
                    bounceTranslate.Y(bounceTranslateOffset);
                }
            } else {
                bounceTranslate.X(0.0);
                bounceTranslate.Y(0.0);
            }
        } else {
            bounceScale.ScaleX(1.0);
            bounceScale.ScaleY(1.0);
            bounceTranslate.X(0.0);
            bounceTranslate.Y(0.0);
        }
    }
}

void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons) {
    if (icons.empty()) return;
    try {
        for (auto& iconInfo : icons) {
            auto element = iconInfo.element.get();
            if (!element) continue;
            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() < 4) continue;
            auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
            auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
            auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
            auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

            if (waveScale) {
                waveScale.ScaleX(1.0);
                waveScale.ScaleY(1.0);
            }
            if (waveTranslate) {
                waveTranslate.X(0.0);
                waveTranslate.Y(0.0);
            }
            if (bounceScale) {
                bounceScale.ScaleX(1.0);
                bounceScale.ScaleY(1.0);
            }
            if (bounceTranslate) {
                bounceTranslate.X(0.0);
                bounceTranslate.Y(0.0);
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error in ResetAllIconScales: %s", e.message().c_str());
    }
}

// Event handlers
void OnTaskbarPointerMoved(void* pThis_key, Input::PointerRoutedEventArgs const& args) {
    try {
        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) return;
        auto& ctx = it->second;
        auto frame = ctx.taskbarFrame.get();
        if (!frame) return;       
        g_isMouseInside = true;
        g_activeContextKey = pThis_key;
        
        UpdateTaskbarEdge(ctx);
        
        double mousePos = ctx.isVertical 
            ? args.GetCurrentPoint(frame).Position().Y 
            : args.GetCurrentPoint(frame).Position().X;

        if (std::abs(mousePos - g_lastMouseX.load()) > MOUSE_STOP_THRESHOLD) {
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
        }
        g_lastMouseX = mousePos;
        if (!g_isRenderingHooked.exchange(true)) {
            g_lastRenderTime = std::chrono::steady_clock::now();
            g_renderingToken = Media::CompositionTarget::Rendering(OnCompositionTargetRendering);
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
            Wh_Log(L"DockAnimation: Started render loop (FocusIn).");
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerMoved: %s", e.message().c_str());
    }
}

void OnTaskbarPointerExited(void* pThis_key) {
    try {
        if (g_activeContextKey.load() == pThis_key) {
            g_isMouseInside = false;
            g_isBouncing = false;
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerExited: %s", e.message().c_str());
    }
}

FrameworkElement FindChildByClassNamePartial(FrameworkElement element, PCWSTR partialName) {
    if (!element) return nullptr;
    return EnumChildElements(element, [partialName](FrameworkElement child) {
        auto className = winrt::get_class_name(child);
        return std::wstring_view(className).find(partialName) != std::wstring_view::npos;
    });
}

static FrameworkElement FindIconHost(FrameworkElement const& taskbarFrame) {
    FrameworkElement host = FindChildByClassNamePartial(taskbarFrame, L"TaskbarFrameRepeater");
    if (!host) host = FindChildByClassName(taskbarFrame, L"TaskbarFrameRepeater");
    if (!host) host = FindChildByClassName(taskbarFrame, L"TaskbarItemHost");
    return host;
}

static uint64_t Fnv1aMix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

static HostSignature ComputeHostSignature(FrameworkElement const& host, bool isVertical) {
    HostSignature sig;
    sig.count = VisualTreeHelper::GetChildrenCount(host);
    sig.width = isVertical ? host.ActualHeight() : host.ActualWidth();

    uint64_t h = 1469598103934665603ULL;
    for (int i = 0; i < sig.count; i++) {
        auto child = VisualTreeHelper::GetChild(host, i).try_as<FrameworkElement>();
        if (!child) continue;
        h = Fnv1aMix(h, (uint64_t)winrt::get_abi(child));
        double size = isVertical ? child.ActualHeight() : child.ActualWidth();
        h = Fnv1aMix(h, (uint64_t)(size * 10.0));
    }
    sig.orderHash = h;
    return sig;
}

static bool SigDifferent(HostSignature const& a, HostSignature const& b) {
    if (a.count != b.count) return true;
    if (std::abs(a.width - b.width) > 0.5) return true;
    if (a.orderHash != b.orderHash) return true;
    return false;
}

enum class ButtonKind {
    App,
    Start,
    Search,
    TaskView,
    Widgets,
    Weather,
    SystemOther
};

static std::wstring W(winrt::hstring const& s) {
    return std::wstring(s.c_str());
}

static std::wstring ToLower(std::wstring s) {
    for (auto& ch : s) ch = (wchar_t)towlower(ch);
    return s;
}

static ButtonKind ClassifyButton(FrameworkElement const& e) {
    if (!e) return ButtonKind::SystemOther;

    std::wstring cn     = W(winrt::get_class_name(e));
    std::wstring feName = W(e.Name());
    std::wstring aid    = W(AutomationProperties::GetAutomationId(e));

    std::wstring cnL = ToLower(cn);
    std::wstring aidL = ToLower(aid);
    std::wstring feL = ToLower(feName);

    auto matches = [&](const wchar_t* s) {
        return cnL.find(s) != std::wstring::npos ||
               aidL.find(s) != std::wstring::npos ||
               feL.find(s) != std::wstring::npos;
    };

    if (matches(L"startbutton")) return ButtonKind::Start;
    if (matches(L"searchbutton")) return ButtonKind::Search;
    if (matches(L"taskviewbutton")) return ButtonKind::TaskView;
    if (matches(L"widgetsbutton")) return ButtonKind::Widgets;
    if (matches(L"weather")) return ButtonKind::Weather;

    if (matches(L"appid:")) return ButtonKind::App;

    {
        if (cnL == L"taskbar.tasklistbutton" ||
            (cnL.size() >= 13 && cnL.rfind(L".tasklistbutton") == cnL.size() - 13)) {
            return ButtonKind::App;
        }
    }

    return ButtonKind::SystemOther;
}


static bool ShouldAnimateElement(FrameworkElement const& e) {
    const int mode = g_settings.excludeSystemButtonsMode;
    if (mode == 0) return true;

    ButtonKind k = ClassifyButton(e);

    if (mode == 2) return k == ButtonKind::App;
    if (mode == 1) return k != ButtonKind::Start;

    return true;
}

static void ResetElementTransforms(FrameworkElement const& element) {
    auto tg = element.RenderTransform().try_as<TransformGroup>();
    if (!tg || tg.Children().Size() < 4) return;

    auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
    auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
    auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
    auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

    if (waveScale) { waveScale.ScaleX(1.0); waveScale.ScaleY(1.0); }
    if (waveTranslate) { waveTranslate.X(0.0); waveTranslate.Y(0.0); }
    if (bounceScale) { bounceScale.ScaleX(1.0); bounceScale.ScaleY(1.0); }
    if (bounceTranslate) { bounceTranslate.X(0.0); bounceTranslate.Y(0.0); }
}

static bool RebaseIconGeometryFast(DockAnimationContext& ctx) {
    auto frame = ctx.taskbarFrame.get();
    if (!frame) return false;

    bool changed = false;

    for (auto it = ctx.icons.begin(); it != ctx.icons.end(); ) {
        if (!it->element.get()) {
            it = ctx.icons.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }

    for (auto& info : ctx.icons) {
        auto e = info.element.get();
        if (!e) continue;

        try {
            double oldX = info.originalCenterX;
            double oldW = info.elementWidth;

            auto t = e.TransformToVisual(frame);
            auto pt = t.TransformPoint({0, 0});
            double w = ctx.isVertical ? e.ActualHeight() : e.ActualWidth();
            info.elementWidth = w;

            float xOrigin = 0.5f;
            float yOrigin = 0.5f;
            if (ctx.isVertical) {
                xOrigin = (ctx.edge == TaskbarEdge::Left) ? 0.0f : 1.0f;
            } else {
                bool isTop = (ctx.edge == TaskbarEdge::Top);
                yOrigin = g_settings.disableVerticalBounce ? 0.5f : (isTop ? 0.0f : 1.0f);
            }

            if (g_settings.taskbarLabelsMode) {
                double newStartX = ctx.isVertical ? pt.Y : pt.X;
                info.originalCenterX = newStartX;

                float iconCenterPx = 24.0f;
                float iconCenterProportion = (w > 0.0) ? (iconCenterPx / (float)w) : 0.5f;
                if (ctx.isVertical) {
                    e.RenderTransformOrigin({ xOrigin, iconCenterProportion });
                } else {
                    e.RenderTransformOrigin({ iconCenterProportion, yOrigin });
                }

                if (std::abs(newStartX - oldX) > 0.5) changed = true;
            } else {
                double newCenterX = ctx.isVertical ? (pt.Y + (w / 2.0)) : (pt.X + (w / 2.0));
                info.originalCenterX = newCenterX;

                if (ctx.isVertical) {
                    e.RenderTransformOrigin({ xOrigin, yOrigin });
                } else {
                    e.RenderTransformOrigin({ 0.5f, yOrigin });
                }

                if (std::abs(newCenterX - oldX) > 0.5) changed = true;
            }

            if (std::abs(w - oldW) > 0.5) changed = true;
        } catch (...) {
        }
    }

    if (changed && ctx.icons.size() > 1) {
        std::sort(ctx.icons.begin(), ctx.icons.end(),
            [](TaskbarIconInfo const& a, TaskbarIconInfo const& b) {
                return a.originalCenterX < b.originalCenterX;
            });
    }

    return changed;
}

// Builds or refreshes the icon list and sets up transforms for animation (scale + translate).
void RefreshIconPositions(DockAnimationContext& ctx) {
    // preserve state
    std::unordered_map<void*, IconAnimState> prevStates;
    std::unordered_set<void*> prevElements;

    for (auto& old : ctx.icons) {
        if (auto e = old.element.get()) {
            void* key = winrt::get_abi(e);
            prevStates[key] = old.state;
            prevElements.insert(key);
        }
    }

    std::vector<TaskbarIconInfo> newIcons;

    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;

    auto IsTaskListButtonExact = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;
        return winrt::get_class_name(e) == L"Taskbar.TaskListButton";
    };

    auto IsTaskListButtonPanel = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;
        return winrt::get_class_name(e) == L"Taskbar.TaskListButtonPanel";
    };

    std::function<FrameworkElement(FrameworkElement)> FindFirstTaskListButton =
        [&](FrameworkElement root) -> FrameworkElement {
            if (!root) return nullptr;
            if (IsTaskListButtonExact(root)) return root;

            int c = VisualTreeHelper::GetChildrenCount(root);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                if (!ch) continue;
                if (IsTaskListButtonExact(ch)) return ch;
                auto deep = FindFirstTaskListButton(ch);
                if (deep) return deep;
            }
            return nullptr;
        };

    // classifier node
    std::function<FrameworkElement(FrameworkElement)> FindClassifierNode =
        [&](FrameworkElement root) -> FrameworkElement {
            if (!root) return nullptr;

            auto aid = W(AutomationProperties::GetAutomationId(root));
            auto nm  = W(AutomationProperties::GetName(root));
            if (!aid.empty() || !nm.empty()) return root;

            int c = VisualTreeHelper::GetChildrenCount(root);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                if (!ch) continue;
                auto hit = FindClassifierNode(ch);
                if (hit) return hit;
            }
            return nullptr;
        };

    // candidate in fallback: TaskListButton + TaskListButtonPanel
    auto IsFallbackCandidate = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;

        if (IsTaskListButtonExact(e) || IsTaskListButtonPanel(e)) return true;

        auto cn = winrt::get_class_name(e);
        if (std::wstring_view(cn).find(L"Taskbar.") != std::wstring_view::npos) {
            auto aid = W(AutomationProperties::GetAutomationId(e));
            auto nm  = W(AutomationProperties::GetName(e));
            if (!aid.empty() || !nm.empty()) return true;
        }

        return false;
    };

    auto SetupAndAddElement = [&](FrameworkElement const& element) {
        try {
            if (!element) return;

            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() < 4) {
                tg = TransformGroup();
                tg.Children().Append(ScaleTransform());       // 0 waveScale
                tg.Children().Append(TranslateTransform());   // 1 waveTranslate
                tg.Children().Append(ScaleTransform());       // 2 bounceScale
                tg.Children().Append(TranslateTransform());   // 3 bounceTranslate
                element.RenderTransform(tg);
            }

            float xOrigin = 0.5f;
            float yOrigin = 0.5f;
            if (ctx.isVertical) {
                xOrigin = (ctx.edge == TaskbarEdge::Left) ? 0.0f : 1.0f;
            } else {
                bool isTop = (ctx.edge == TaskbarEdge::Top);
                yOrigin = g_settings.disableVerticalBounce ? 0.5f : (isTop ? 0.0f : 1.0f);
            }

            auto transform = element.TransformToVisual(taskbarFrame);
            auto point = transform.TransformPoint({0, 0});

            TaskbarIconInfo info;
            info.element = element;
            info.elementWidth = ctx.isVertical ? element.ActualHeight() : element.ActualWidth();

            if (g_settings.taskbarLabelsMode) {
                float iconCenterPx = 24.0f;
                float iconCenterProportion =
                    (info.elementWidth > 0) ? (iconCenterPx / (float)info.elementWidth) : 0.5f;

                if (ctx.isVertical) {
                    element.RenderTransformOrigin({ xOrigin, iconCenterProportion });
                } else {
                    element.RenderTransformOrigin({ iconCenterProportion, yOrigin });
                }
                info.originalCenterX = ctx.isVertical ? point.Y : point.X;
            } else {
                if (ctx.isVertical) {
                    element.RenderTransformOrigin({ xOrigin, yOrigin });
                } else {
                    element.RenderTransformOrigin({ 0.5f, yOrigin });
                }
                info.originalCenterX = ctx.isVertical ? (point.Y + (info.elementWidth / 2.0)) : (point.X + (info.elementWidth / 2.0));
            }

            void* key = winrt::get_abi(element);

            auto it = prevStates.find(key);
            if (it != prevStates.end()) {
                info.state = it->second;

                auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
                auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
                if (waveScale) {
                    waveScale.ScaleX(info.state.waveScale);
                    waveScale.ScaleY(info.state.waveScale);
                }
                if (waveTranslate) {
                    if (ctx.isVertical) {
                        waveTranslate.Y(info.state.waveTranslateX);
                    } else {
                        waveTranslate.X(info.state.waveTranslateX);
                    }
                }
            }

            newIcons.push_back(info);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"DockAnimation: HRESULT error in SetupAndAddElement: %s", e.message().c_str());
        }
    };

    // SMART host find (structure-based)
    auto FindSmartHost = [&]() -> FrameworkElement {
        std::vector<FrameworkElement> nodes;
        nodes.reserve(512);

        std::vector<FrameworkElement> stack;
        stack.reserve(512);
        stack.push_back(taskbarFrame);

        while (!stack.empty()) {
            auto cur = stack.back();
            stack.pop_back();
            if (!cur) continue;

            nodes.push_back(cur);

            int c = VisualTreeHelper::GetChildrenCount(cur);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(cur, i).try_as<FrameworkElement>();
                if (ch) stack.push_back(ch);
            }
        }

        std::function<bool(FrameworkElement,int)> SubtreeHasIconLike =
            [&](FrameworkElement root, int depth) -> bool {
                if (!root || depth <= 0) return false;
                if (IsTaskListButtonExact(root) || IsTaskListButtonPanel(root)) return true;

                int c = VisualTreeHelper::GetChildrenCount(root);
                for (int i = 0; i < c; i++) {
                    auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                    if (!ch) continue;
                    if (SubtreeHasIconLike(ch, depth - 1)) return true;
                }
                return false;
            };

        FrameworkElement best = nullptr;
        int bestHits = 0;
        double bestWidth = 0.0;

        for (auto const& n : nodes) {
            int c = VisualTreeHelper::GetChildrenCount(n);
            if (c < 3 || c > 120) continue;

            int hits = 0;
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(n, i).try_as<FrameworkElement>();
                if (!ch) continue;
                if (SubtreeHasIconLike(ch, 5)) hits++;
            }

            if (hits >= 3) {
                double w = (double)n.ActualWidth();
                if (hits > bestHits || (hits == bestHits && w > bestWidth)) {
                    best = n;
                    bestHits = hits;
                    bestWidth = w;
                }
            }
        }

        return best;
    };

    // host selection
    auto host = ctx.iconHost.get();
    if (!host) {
        host = FindSmartHost();
        if (host) ctx.iconHost = host;
    }

    try {
        if (host) {
            // MAIN: enumerate host children
            int count = VisualTreeHelper::GetChildrenCount(host);
            for (int i = 0; i < count; i++) {
                auto child = VisualTreeHelper::GetChild(host, i).try_as<FrameworkElement>();
                if (!child) continue;

                FrameworkElement classifier = FindClassifierNode(child);
                FrameworkElement base = classifier ? classifier : child;

                bool animate = (g_settings.excludeSystemButtonsMode == 0)
                    ? true
                    : ShouldAnimateElement(base);

                // target: prefer TaskListButton inside, else child
                FrameworkElement target = FindFirstTaskListButton(child);
                if (!target) target = child;

                if (!animate) {
                    void* kT = winrt::get_abi(target);
                    if (prevElements.contains(kT)) ResetElementTransforms(target);

                    void* kC = winrt::get_abi(child);
                    if (prevElements.contains(kC)) ResetElementTransforms(child);
                    continue;
                }

                SetupAndAddElement(target);
            }

            ctx.lastSig = ComputeHostSignature(host, ctx.isVertical);
        } else {
            // FALLBACK: recurse and include TaskListButtonPanel too
            std::function<void(FrameworkElement)> recurse = [&](FrameworkElement element) {
                if (!element) return;

                if (IsFallbackCandidate(element)) {
                    FrameworkElement classifier = FindClassifierNode(element);
                    FrameworkElement base = classifier ? classifier : element;

                    bool animate = (g_settings.excludeSystemButtonsMode == 0)
                        ? true
                        : ShouldAnimateElement(base);

                    FrameworkElement target = FindFirstTaskListButton(element);
                    if (!target) target = element;

                    if (!animate) {
                        void* k = winrt::get_abi(target);
                        if (prevElements.contains(k)) ResetElementTransforms(target);
                    } else {
                        SetupAndAddElement(target);
                    }
                }

                int c = VisualTreeHelper::GetChildrenCount(element);
                for (int i = 0; i < c; i++) {
                    auto ch = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
                    if (ch) recurse(ch);
                }
            };

            recurse(taskbarFrame);
        }

        std::sort(newIcons.begin(), newIcons.end(),
            [](TaskbarIconInfo const& a, TaskbarIconInfo const& b) {
                return a.originalCenterX < b.originalCenterX;
            });

        ctx.icons = std::move(newIcons);

    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error during icon search: %s", e.message().c_str());
    }
}

void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame) {
    try {
        DockAnimationContext ctx;
        ctx.taskbarFrame = taskbarFrame;
        ctx.isInitialized = false;
        ctx.hWnd = GetCurrentThreadTrayWindow();
        UpdateTaskbarEdge(ctx);
        g_contexts[pThis] = std::move(ctx);
        Wh_Log(L"DockAnimation: Monitor %p registered (hook-based). Edge: %d, HWND: %p", pThis, (int)ctx.edge, ctx.hWnd);
    }
    catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: Failed to initialize context for %p: %s",
               pThis, e.message().c_str());
    }
}

// Main animation loop (pulse)
void OnCompositionTargetRendering(winrt::Windows::Foundation::IInspectable const&,
                                  winrt::Windows::Foundation::IInspectable const&) {
    try {
        auto now = std::chrono::steady_clock::now();

        double deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                                now - g_lastRenderTime)
                                .count();
        g_lastRenderTime = now;

        double dtSec = std::max(0.0, deltaTime) / 1000.0;

        double intensityChange = deltaTime / g_settings.focusDuration;
        double currentIntensity = g_animationIntensity.load();

        if (g_isMouseInside) {
            // FocusIn
            currentIntensity = std::min(1.0, currentIntensity + intensityChange);
        } else {
            // FocusOut
            currentIntensity = std::max(0.0, currentIntensity - intensityChange);
        }

        g_animationIntensity = currentIntensity;

        void* pThis_key = g_activeContextKey.load();
        if (pThis_key == nullptr) {
            if (currentIntensity <= 0.0) {
                Media::CompositionTarget::Rendering(g_renderingToken);
                g_isRenderingHooked = false;
                Wh_Log(L"DockAnimation: Stopped render loop (Key=null, Intensity=0).");
            }
            return;
        }

        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) return;

        auto& ctx = it->second;

        if (!g_isMouseInside && currentIntensity <= 0.0) {
            Media::CompositionTarget::Rendering(g_renderingToken);
            g_isRenderingHooked = false;

            ResetAllIconScales(ctx.icons);
            ctx.isInitialized = false;
            ctx.icons.clear();

            g_activeContextKey = nullptr;
            g_lastMouseX = -1.0;
            g_isBouncing = false;

            Wh_Log(L"DockAnimation: Stopped render loop (FocusOut complete).");
            return;
        }

        if (!ctx.isInitialized) {
            RefreshIconPositions(ctx);
            ctx.isInitialized = true;
        }

        // Dirty-check
        if (g_isMouseInside) {
            auto sinceCheck = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - ctx.lastDirtyCheck).count();

            if (sinceCheck > 120) {
                ctx.lastDirtyCheck = now;

                auto frame = ctx.taskbarFrame.get();
                if (frame) {
                    UpdateTaskbarEdge(ctx);

                    auto host = ctx.iconHost.get();
                    if (!host) {
                        host = FindIconHost(frame);
                        if (host) ctx.iconHost = host;
                    }

                    if (host) {
                        HostSignature sig = ComputeHostSignature(host, ctx.isVertical);

                        if (SigDifferent(sig, ctx.lastSig)) {
                            RefreshIconPositions(ctx);
                        } else {
                            RebaseIconGeometryFast(ctx);
                        }

                        ctx.lastSig = sig;
                    }
                }
            }
        }

        if (ctx.icons.empty()) {
            if (ctx.isInitialized) {
                RefreshIconPositions(ctx);
            }
            if (ctx.icons.empty()) return;
        }

        if (g_settings.disableBounce) {
            g_isBouncing = false;
        } else {
            auto elapsedSinceMove =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - g_lastSignificantMoveTime)
                    .count();

            if (g_isMouseInside && elapsedSinceMove > g_settings.bounceDelay) {
                if (!g_isBouncing.exchange(true)) {
                    g_bounceStartTime = now;
                }
            } else {
                g_isBouncing = false;
            }
        }

        double targetBounceIntensity = g_isBouncing ? 1.0 : 0.0;
        double bounceFadeDuration = g_settings.focusDuration / 1000.0;
        double bounceIntensityChange = dtSec / bounceFadeDuration;
        double currentBounceIntensity = g_bounceIntensity.load();
        if (targetBounceIntensity > currentBounceIntensity) {
            currentBounceIntensity = std::min(targetBounceIntensity, currentBounceIntensity + bounceIntensityChange);
        } else {
            currentBounceIntensity = std::max(targetBounceIntensity, currentBounceIntensity - bounceIntensityChange);
        }
        g_bounceIntensity = currentBounceIntensity;

        double mouseX = g_lastMouseX.load();
        if (mouseX == -1.0) mouseX = 0.0;

        ApplyAnimation(mouseX, ctx, currentIntensity, dtSec);
    } catch (...) {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
        }
    }
}

// Hooking
// Pointer event hooks (new approach: activates mod immediately)
using TaskbarFrame_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerMoved_t TaskbarFrame_OnPointerMoved_Original;

int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskbarFrame_OnPointerMoved_Original(pThis, pArgs);
    };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element)
        return original();
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskbarFrame")
        return original();
    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(), winrt::put_abi(args));
    if (!args)
        return original();
    // Initialize animation hooks only once
    if (g_contexts.find(pThis) == g_contexts.end()) {
        InitializeAnimationHooks(pThis, element);
        Wh_Log(L"DockAnimation: Initialized via OnPointerMoved (%s)", className.c_str());
    }
    // Forward event to existing logic
    OnTaskbarPointerMoved(pThis, args);
    return original();
}

using TaskbarFrame_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerExited_t TaskbarFrame_OnPointerExited_Original;

int WINAPI TaskbarFrame_OnPointerExited_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskbarFrame_OnPointerExited_Original(pThis, pArgs);
    };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element)
        return original();
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskbarFrame")
        return original();
    OnTaskbarPointerExited(pThis);
    return original();
}

void LoadSettings() {
    int rawScale = Wh_GetIntSetting(L"MaxScale");

    if (rawScale < 101) {
        rawScale = 101;
    } else if (rawScale > 220) {
        rawScale = 220;
    }

    g_settings.animationType = Wh_GetIntSetting(L"AnimationType");
    g_settings.maxScale = (double)rawScale / 100.0;

    g_settings.effectRadius = Wh_GetIntSetting(L"EffectRadius");
    if (g_settings.effectRadius <= 0) g_settings.effectRadius = 100;

    g_settings.spacingFactor = (double)Wh_GetIntSetting(L"SpacingFactor") / 100.0;
    if (g_settings.spacingFactor < 0.0) g_settings.spacingFactor = 0.5;

    g_settings.bounceDelay = Wh_GetIntSetting(L"BounceDelay");
    if (g_settings.bounceDelay < 0) g_settings.bounceDelay = 100;

    g_settings.focusDuration = (double)Wh_GetIntSetting(L"FocusDuration");
    if (g_settings.focusDuration <= 0) g_settings.focusDuration = 150.0;

    g_settings.disableVerticalBounce = (bool)Wh_GetIntSetting(L"DisableVerticalBounce");
    g_settings.taskbarLabelsMode = (bool)Wh_GetIntSetting(L"TaskbarLabelsMode");
    g_settings.excludeSystemButtonsMode = Wh_GetIntSetting(L"ExcludeSystemButtonsMode");

    g_settings.lerpSpeed = (double)Wh_GetIntSetting(L"LerpSpeed");
    if (g_settings.lerpSpeed < 0) g_settings.lerpSpeed = 0;
    if (g_settings.lerpSpeed > 60) g_settings.lerpSpeed = 60;

    g_settings.disableBounce = (bool)Wh_GetIntSetting(L"DisableBounce");

    double bounceAmp = (double)Wh_GetIntSetting(L"BounceAmplitude");
    if (bounceAmp < 50.0) {
        bounceAmp = 50.0;
    } else if (bounceAmp > 300.0) {
        bounceAmp = 300.0;
    }
    g_settings.bounceAmplitude = bounceAmp / 100.0;
    g_settings.antiClipDisabled =
        (bool)Wh_GetIntSetting(L"DisableAntiClipGuard");

    Wh_Log(L"DockAnimation: Settings loaded.");
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandle(L"ExplorerExtensions.dll");
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"
            },
            &TaskbarFrame_OnPointerMoved_Original,
            TaskbarFrame_OnPointerMoved_Hook,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"
            },
            &TaskbarFrame_OnPointerExited_Original,
            TaskbarFrame_OnPointerExited_Hook,
        },
    };
    if (!HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"DockAnimation: HookSymbols failed.");
        return false;
    }
    Wh_Log(L"DockAnimation: HookSymbols succeeded (Pointer events only).");
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

// Lazy hook path (if Taskbar.View.dll loads later)
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) return module;

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module) {
        if (!g_taskbarViewDllLoaded.exchange(true)) {
            Wh_Log(L"DockAnimation: Taskbar.View.dll loaded, hooking symbols...");
            if (HookTaskbarViewDllSymbols(module)) {
                if (!g_hooksApplied.exchange(true)) {
                    Wh_ApplyHookOperations();
                    Wh_Log(L"DockAnimation: Hooks applied (slow path).");
                }
            }
        }
    }
    return module;
}

// ============================================================================
// 任务栏秒显（自动隐藏即时显示）
//
// 来自 CW-任务栏秒显（Taskbar Auto-Hide Instant Show 2.3，Bo0ii），代码逻辑与
// 上游 2.2 逐字节一致，仅做两类机械改名以并入本模组：
//   1. 入口改名：Wh_ModInit→Init、Wh_ModAfterInit→AfterInit、
//      Wh_ModUninit→Uninit、Wh_ModSettingsChanged→SettingsChanged；
//   2. 设置键加 autoHide. 前缀（animationType→autoHide.animationType 等），
//      与图标动画设置隔离（Windows 注册表值名不区分大小写，宿主有 AnimationType、
//      秒显有 animationType，不加前缀会互相覆盖）。
// 入口由本模组的 Wh_ModInit / Wh_ModAfterInit / Wh_ModUninit /
// Wh_ModSettingsChanged 统一调用。
// ============================================================================
namespace TaskbarAutoHide {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct {
    int animationType;
    int showSpeedup;
    int hideSpeedup;
    int showDuration;
    int hideDuration;
    int frameRate;
    int unhideDelay;
    int hideDelay;
    bool oldTaskbarOnWin11;
    bool edgeDetection;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

double g_recipCyclesPerSecond;

std::atomic<DWORD> g_slideWindowThreadId;
int g_slideWindowSpeedup;
double g_slideWindowStartTime;
double g_slideWindowLastFrameStartTime;
std::atomic<int> g_animationGeneration{0};

void TimerInitialize() {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double cyclesPerSecond = static_cast<double>(freq.QuadPart);
    g_recipCyclesPerSecond = 1.0 / cyclesPerSecond;
}

double TimerGetCycles() {
    LARGE_INTEGER T1;
    QueryPerformanceCounter(&T1);
    return static_cast<double>(T1.QuadPart);
}

double TimerGetSeconds() {
    return TimerGetCycles() * g_recipCyclesPerSecond;
}

double EaseOutElastic(double t) {
    if (t <= 0.0) return 0.0;
    if (t >= 1.0) return 1.0;
    double p = 0.3;
    double s = p / 4.0;
    return pow(2.0, -10.0 * t) * sin((t - s) * (2.0 * M_PI) / p) + 1.0;
}

double EaseOutBounce(double t) {
    if (t < 1.0 / 2.75) {
        return 7.5625 * t * t;
    } else if (t < 2.0 / 2.75) {
        t -= 1.5 / 2.75;
        return 7.5625 * t * t + 0.75;
    } else if (t < 2.5 / 2.75) {
        t -= 2.25 / 2.75;
        return 7.5625 * t * t + 0.9375;
    } else {
        t -= 2.625 / 2.75;
        return 7.5625 * t * t + 0.984375;
    }
}

double EaseOutCubic(double t) {
    double t1 = t - 1.0;
    return t1 * t1 * t1 + 1.0;
}

double EaseInCubic(double t) {
    return t * t * t;
}

double EaseOutQuint(double t) {
    double t1 = 1.0 - t;
    return 1.0 - t1 * t1 * t1 * t1 * t1;
}

double EaseInQuint(double t) {
    return t * t * t * t * t;
}

double EaseOutBack(double t) {
    const double c1 = 1.70158;
    const double c3 = c1 + 1.0;
    double t1 = t - 1.0;
    return 1.0 + c3 * t1 * t1 * t1 + c1 * t1 * t1;
}

double EaseInBack(double t) {
    const double c1 = 1.70158;
    const double c3 = c1 + 1.0;
    return c3 * t * t * t - c1 * t * t;
}

using GetTickCount_t = decltype(&GetTickCount);
GetTickCount_t GetTickCount_Original;
DWORD WINAPI GetTickCount_Hook() {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        return GetTickCount_Original();
    }

    DWORD ms = (DWORD)((g_slideWindowLastFrameStartTime -
                         g_slideWindowStartTime) *
                            1000.0 * (g_slideWindowSpeedup / 100.0) +
                        0.5);

    Wh_Log(L"> %u", ms);

    return ms;
}

using Sleep_t = decltype(&Sleep);
Sleep_t Sleep_Original;
void WINAPI Sleep_Hook(DWORD dwMilliseconds) {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        Sleep_Original(dwMilliseconds);
        return;
    }

    double frameTotalTime = 1000.0 / g_settings.frameRate;

    double frameElapsedTime =
        (TimerGetSeconds() - g_slideWindowLastFrameStartTime) * 1000.0;

    int sleepTime = (int)(frameTotalTime - frameElapsedTime + 0.5);

    Wh_Log(L"> %f - %f = %d", frameTotalTime, frameElapsedTime, sleepTime);

    if (sleepTime > 0) {
        Sleep_Original(sleepTime);
    }

    g_slideWindowLastFrameStartTime = TimerGetSeconds();
}

static int Lerp(int a, int b, double t) {
    return a + (int)((b - a) * t);
}

void DoCustomAnimation(HWND hWnd,
                       const RECT* startRect,
                       const RECT* endRect,
                       bool show,
                       int myAnimGen) {
    int animType = g_settings.animationType;
    int duration = show ? g_settings.showDuration : g_settings.hideDuration;

    if (duration <= 0) {
        SetWindowPos(hWnd, NULL, endRect->left, endRect->top,
                     endRect->right - endRect->left,
                     endRect->bottom - endRect->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        return;
    }

    double frameDuration = 1000.0 / g_settings.frameRate;
    int totalFrames = (int)(duration / frameDuration);
    if (totalFrames < 1) totalFrames = 1;

    bool usePosition = (animType == 1 || animType == 2 || animType == 4 ||
                        animType == 5);
    bool useFade = (animType == 3 || animType == 4);

    LONG_PTR originalExStyle = 0;
    if (useFade) {
        originalExStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE,
                             originalExStyle | WS_EX_LAYERED);
        }
        SetLayeredWindowAttributes(hWnd, 0, show ? 0 : 255, LWA_ALPHA);
        DwmFlush();
    }

    if (useFade && !usePosition && show) {
        SetWindowPos(hWnd, NULL, endRect->left, endRect->top,
                     endRect->right - endRect->left,
                     endRect->bottom - endRect->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    double startTime = TimerGetSeconds();

    for (int i = 1; i <= totalFrames; i++) {
        if (g_animationGeneration.load() != myAnimGen) {
            break;
        }

        double t = (double)i / totalFrames;

        double posT = t;
        double alphaT = t;

        if (show) {
            switch (animType) {
                case 1:
                    posT = EaseOutElastic(t);
                    break;
                case 2:
                    posT = EaseOutBounce(t);
                    break;
                case 3:
                    alphaT = EaseOutCubic(t);
                    break;
                case 4:
                    posT = EaseOutQuint(t);
                    alphaT = EaseOutQuint(t);
                    break;
                case 5:
                    posT = EaseOutBack(t);
                    break;
            }
        } else {
            switch (animType) {
                case 1:
                    posT = EaseInCubic(t);
                    break;
                case 2:
                    posT = EaseInCubic(t);
                    break;
                case 3:
                    alphaT = EaseInCubic(t);
                    break;
                case 4:
                    posT = EaseInQuint(t);
                    alphaT = EaseInQuint(t);
                    break;
                case 5:
                    posT = EaseInBack(t);
                    break;
            }
        }

        if (usePosition) {
            int x = Lerp(startRect->left, endRect->left, posT);
            int y = Lerp(startRect->top, endRect->top, posT);
            int w = Lerp(startRect->right - startRect->left,
                         endRect->right - endRect->left, posT);
            int h = Lerp(startRect->bottom - startRect->top,
                         endRect->bottom - endRect->top, posT);
            SetWindowPos(hWnd, NULL, x, y, w, h,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }

        if (useFade) {
            BYTE alpha;
            if (show) {
                alpha = (BYTE)(255.0 * alphaT);
            } else {
                alpha = (BYTE)(255.0 * (1.0 - alphaT));
            }
            if (alpha < 1) alpha = 1;
            SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
            DwmFlush();
        }

        if (!useFade) {
            double targetTime =
                startTime + (double)i * frameDuration / 1000.0;
            double currentTime = TimerGetSeconds();
            double sleepMs = (targetTime - currentTime) * 1000.0;
            if (sleepMs > 1.0) {
                Sleep_Original((DWORD)(sleepMs + 0.5));
            }
        }
    }

    if (usePosition) {
        SetWindowPos(hWnd, NULL, endRect->left, endRect->top,
                     endRect->right - endRect->left,
                     endRect->bottom - endRect->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    if (useFade) {
        if (!show) {
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            DwmFlush();
            SetWindowPos(hWnd, NULL, endRect->left, endRect->top,
                         endRect->right - endRect->left,
                         endRect->bottom - endRect->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            DwmFlush();
        }

        // Remove WS_EX_LAYERED while window is either:
        //   - off-screen (hide) — no flash possible
        //   - at full alpha (show) — seamless transition
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE, originalExStyle);
            DwmFlush();
        }

        // Only restore full alpha if window was already layered
        if (show && (originalExStyle & WS_EX_LAYERED)) {
            SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        }
    }
}

using TrayUI_SlideWindow_t = void(WINAPI*)(void* pThis,
                                           HWND hWnd,
                                           const RECT* rect,
                                           HMONITOR monitor,
                                           bool show,
                                           bool animate);
TrayUI_SlideWindow_t TrayUI_SlideWindow_Original;
void WINAPI TrayUI_SlideWindow_Hook(void* pThis,
                                    HWND hWnd,
                                    const RECT* rect,
                                    HMONITOR monitor,
                                    bool show,
                                    bool animate) {
    Wh_Log(L"> show=%d animType=%d", show, g_settings.animationType);

    if (g_settings.animationType == -1) {
        // No animation — snap to final position instantly.
        TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, false);
    } else if (g_settings.animationType == 0) {
        g_slideWindowSpeedup =
            show ? g_settings.showSpeedup : g_settings.hideSpeedup;
        g_slideWindowStartTime = TimerGetSeconds();
        g_slideWindowLastFrameStartTime = g_slideWindowStartTime;
        g_slideWindowThreadId = GetCurrentThreadId();

        TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, animate);

        g_slideWindowThreadId = 0;
    } else {
        int animGen = ++g_animationGeneration;

        RECT startRect;
        GetWindowRect(hWnd, &startRect);

        if (show) {
            // Suppress pre-animation flash: hide window, let the
            // original update internal TrayUI state invisibly, then
            // restore the start position for our custom animation.
            LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
            bool addedLayered = !(exStyle & WS_EX_LAYERED);
            if (addedLayered)
                SetWindowLongPtr(hWnd, GWL_EXSTYLE,
                                 exStyle | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            DwmFlush();

            TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show,
                                        false);

            // Move back to hidden start position
            SetWindowPos(hWnd, NULL, startRect.left, startRect.top,
                         startRect.right - startRect.left,
                         startRect.bottom - startRect.top,
                         SWP_NOZORDER | SWP_NOACTIVATE);

            // Restore window style before custom animation
            if (addedLayered) {
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);
            } else {
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            }
            DwmFlush();

            DoCustomAnimation(hWnd, &startRect, rect, show, animGen);
        } else {
            DoCustomAnimation(hWnd, &startRect, rect, show, animGen);

            TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show,
                                        false);
        }
    }
}

using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;
UINT_PTR WINAPI SetTimer_Hook(HWND hWnd,
                              UINT_PTR nIDEvent,
                              UINT uElapse,
                              TIMERPROC lpTimerFunc) {
    if (hWnd && (nIDEvent == 2 || nIDEvent == 3)) {
        wchar_t className[64];
        if (GetClassName(hWnd, className, ARRAYSIZE(className)) > 0) {
            if (wcscmp(className, L"Shell_TrayWnd") == 0 ||
                wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                uElapse = (nIDEvent == 3) ? g_settings.unhideDelay
                                          : g_settings.hideDelay;
            }
        }
    }
    return SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

// --- Edge detection for empty monitors ---

HANDLE g_edgeCheckThread = NULL;
std::atomic<bool> g_edgeCheckRunning{false};

HWND FindTaskbarForMonitor(HMONITOR hMon) {
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (hTaskbar) {
        HMONITOR tbMon =
            MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTONEAREST);
        if (tbMon == hMon)
            return hTaskbar;
    }

    HWND hSecondary = NULL;
    while ((hSecondary = FindWindowEx(NULL, hSecondary,
                                      L"Shell_SecondaryTrayWnd", NULL))) {
        HMONITOR tbMon =
            MonitorFromWindow(hSecondary, MONITOR_DEFAULTTONEAREST);
        if (tbMon == hMon)
            return hSecondary;
    }
    return NULL;
}

bool IsTaskbarEffectivelyHidden(HWND hTaskbar, HMONITOR hMon) {
    RECT tr;
    GetWindowRect(hTaskbar, &tr);

    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi))
        return false;

    RECT overlap;
    if (!IntersectRect(&overlap, &tr, &mi.rcMonitor))
        return true;

    int visH = overlap.bottom - overlap.top;
    int visW = overlap.right - overlap.left;
    int tbH = tr.bottom - tr.top;
    int tbW = tr.right - tr.left;

    // Hidden if less than half visible in either dimension
    return (tbH > 0 && visH < tbH / 2) || (tbW > 0 && visW < tbW / 2);
}

bool IsCursorAtMonitorEdge(POINT pt, HMONITOR hMon) {
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi))
        return false;

    return pt.y >= mi.rcMonitor.bottom - 1 ||
           pt.y <= mi.rcMonitor.top ||
           pt.x >= mi.rcMonitor.right - 1 ||
           pt.x <= mi.rcMonitor.left;
}

DWORD WINAPI EdgeCheckThreadProc(LPVOID) {
    // Fast poll so we notice the cursor hitting the edge immediately.
    const DWORD pollInterval = 20;
    // Re-post the unhide trigger while the cursor sits at the edge and
    // the taskbar is still hidden. No one-shot latch — a single
    // PostMessage can be swallowed by foreground apps (fullscreen Chrome,
    // terminals, games), so we keep retrying until it takes or the cursor
    // leaves.
    const DWORD retryInterval = 150;

    DWORD lastRetryTime = 0;

    while (g_edgeCheckRunning) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            HMONITOR hMon =
                MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

            if (IsCursorAtMonitorEdge(pt, hMon)) {
                DWORD now = GetTickCount();
                if (lastRetryTime == 0 ||
                    now - lastRetryTime >= retryInterval) {
                    HWND hTaskbar = FindTaskbarForMonitor(hMon);
                    if (hTaskbar &&
                        IsTaskbarEffectivelyHidden(hTaskbar, hMon)) {
                        PostMessage(hTaskbar, WM_TIMER, 3, 0);
                    }
                    lastRetryTime = now ? now : 1;
                }
            } else {
                lastRetryTime = 0;
            }
        }

        Sleep_Original ? Sleep_Original(pollInterval)
                       : Sleep(pollInterval);
    }

    return 0;
}

void StartEdgeCheckThread() {
    if (g_edgeCheckThread)
        return;

    g_edgeCheckRunning = true;
    g_edgeCheckThread =
        CreateThread(NULL, 0, EdgeCheckThreadProc, NULL, 0, NULL);
}

void StopEdgeCheckThread() {
    if (!g_edgeCheckThread)
        return;

    g_edgeCheckRunning = false;
    WaitForSingleObject(g_edgeCheckThread, 2000);
    CloseHandle(g_edgeCheckThread);
    g_edgeCheckThread = NULL;
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibrary(L"taskbar.dll");
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // explorer.exe, taskbar.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual void __cdecl TrayUI::SlideWindow(struct HWND__ *,struct tagRECT const *,struct HMONITOR__ *,bool,bool))"},
            &TrayUI_SlideWindow_Original,
            TrayUI_SlideWindow_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(?SlideWindow@TrayUI@@UEAAXPEAUHWND__@@PEBUtagRECT@@PEAUHMONITOR__@@_N3@Z)",
         &TrayUI_SlideWindow_Original, TrayUI_SlideWindow_Hook},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }

    return module;
}

void LoadSettings() {
    LPCWSTR animType = Wh_GetStringSetting(L"autoHide.animationType");
    if (wcscmp(animType, L"none") == 0) {
        g_settings.animationType = -1;
    } else if (wcscmp(animType, L"elastic") == 0) {
        g_settings.animationType = 1;
    } else if (wcscmp(animType, L"bounce") == 0) {
        g_settings.animationType = 2;
    } else if (wcscmp(animType, L"fade") == 0) {
        g_settings.animationType = 3;
    } else if (wcscmp(animType, L"slideFade") == 0) {
        g_settings.animationType = 4;
    } else if (wcscmp(animType, L"overshoot") == 0) {
        g_settings.animationType = 5;
    } else {
        g_settings.animationType = 0;
    }
    Wh_FreeStringSetting(animType);

    g_settings.showSpeedup = Wh_GetIntSetting(L"autoHide.showSpeedup");
    g_settings.hideSpeedup = Wh_GetIntSetting(L"autoHide.hideSpeedup");
    g_settings.showDuration = Wh_GetIntSetting(L"autoHide.showDuration");
    g_settings.hideDuration = Wh_GetIntSetting(L"autoHide.hideDuration");
    g_settings.frameRate = Wh_GetIntSetting(L"autoHide.frameRate");
    g_settings.unhideDelay = Wh_GetIntSetting(L"autoHide.unhideDelay");
    g_settings.hideDelay = Wh_GetIntSetting(L"autoHide.hideDelay");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"autoHide.oldTaskbarOnWin11");
    g_settings.edgeDetection = Wh_GetIntSetting(L"autoHide.edgeDetection");
}

BOOL Init() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_settings.oldTaskbarOnWin11) {
        bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

        if (g_winVersion >= WinVersion::Win11) {
            g_winVersion = WinVersion::Win10;
        }

        if (hasWin10Taskbar && !HookTaskbarSymbols()) {
            return FALSE;
        }
    } else if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");

    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    auto pKernelBaseGetTickCount = (decltype(&GetTickCount))GetProcAddress(
        kernelBaseModule, "GetTickCount");
    WindhawkUtils::Wh_SetFunctionHookT(
        pKernelBaseGetTickCount, GetTickCount_Hook, &GetTickCount_Original);

    auto pKernelBaseSleep =
        (decltype(&Sleep))GetProcAddress(kernelBaseModule, "Sleep");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseSleep, Sleep_Hook,
                                       &Sleep_Original);

    HMODULE user32Module = GetModuleHandle(L"user32.dll");
    auto pSetTimer =
        (decltype(&SetTimer))GetProcAddress(user32Module, "SetTimer");
    WindhawkUtils::Wh_SetFunctionHookT(pSetTimer, SetTimer_Hook,
                                       &SetTimer_Original);

    TimerInitialize();

    g_initialized = true;

    if (g_settings.edgeDetection) {
        StartEdgeCheckThread();
    }

    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");

    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }
}

void Uninit() {
    Wh_Log(L">");
    StopEdgeCheckThread();
}

BOOL SettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    if (g_settings.edgeDetection) {
        StartEdgeCheckThread();
    } else {
        StopEdgeCheckThread();
    }

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}

}  // namespace TaskbarAutoHide

// Windhawk entry points
BOOL Wh_ModInit() {
    Wh_Log(L"DockAnimation: Wh_ModInit");
    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) return FALSE;
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(
            pKernelBaseLibraryExW,
            LoadLibraryExW_Hook,
            &LoadLibraryExW_Original);
    }

    // 任务栏秒显：初始化失败时只记录日志，不阻断图标动画。
    if (!TaskbarAutoHide::Init()) {
        Wh_Log(L"任务栏秒显初始化失败，继续仅图标动画");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"DockAnimation: Wh_ModAfterInit");

    if (g_taskbarViewDllLoaded) {
        g_hooksApplied = true;
        Wh_Log(L"DockAnimation: Hooks already applied by Windhawk (fast path).");
    }

    TaskbarAutoHide::AfterInit();
}

typedef void (*RunFromWindowThreadProc_t)(PVOID);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);
    return true;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"DockAnimation: Wh_ModBeforeUninit (safe cleanup)");

    g_activeContextKey = nullptr;
    g_isBouncing = false;
    g_bounceIntensity = 0.0;
    g_isMouseInside = false;
    g_animationIntensity = 0.0;

    try {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
            Wh_Log(L"DockAnimation: Unhooked CompositionTarget::Rendering.");
        }
    }
    catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error unhooking rendering: %s",
               e.message().c_str());
    }

    // Group elements by the thread/window that owns them to prevent COM thread violations
    std::map<HWND, std::vector<winrt::weak_ref<FrameworkElement>>> windowIcons;
    for (auto& pair : g_contexts) {
        auto& ctx = pair.second;
        HWND hWnd = ctx.hWnd ? ctx.hWnd : FindWindow(L"Shell_TrayWnd", NULL);
        if (hWnd) {
            auto& vec = windowIcons[hWnd];
            for (auto& icon : ctx.icons) {
                vec.push_back(icon.element);
            }
        }
    }

    for (auto& pair : windowIcons) {
        HWND hWnd = pair.first;
        std::vector<winrt::weak_ref<FrameworkElement>> elements = std::move(pair.second);
        std::function<void()> action = [elements = std::move(elements)]() {
            try {
                for (auto& weak_el : elements) {
                    if (auto element = weak_el.get()) {
                        ResetElementTransforms(element);
                    }
                }
            }
            catch (...) {}
        };
        RunFromWindowThread(
            hWnd,
            [](PVOID p) {
                auto* fn = static_cast<std::function<void()>*>(p);
                (*fn)();
            },
            &action);
    }

    g_contexts.clear();
    g_taskbarViewDllLoaded = false;
    g_hooksApplied = false;
}

void Wh_ModUninit() {
    Wh_Log(L"TaskbarAutoHide: Wh_ModUninit");
    TaskbarAutoHide::Uninit();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"DockAnimation: Settings changed.");
    LoadSettings();

    // Group context pointers by their HWND to prevent COM thread violations
    std::map<HWND, std::vector<DockAnimationContext*>> windowContexts;
    for (auto& pair : g_contexts) {
        auto& ctx = pair.second;
        HWND hWnd = ctx.hWnd ? ctx.hWnd : FindWindow(L"Shell_TrayWnd", NULL);
        if (hWnd) {
            windowContexts[hWnd].push_back(&ctx);
        }
    }

    for (auto& pair : windowContexts) {
        HWND hWnd = pair.first;
        std::vector<DockAnimationContext*> ctxs = std::move(pair.second);
        std::function<void()> action = [ctxs = std::move(ctxs)]() {
            try {
                for (auto* ctx : ctxs) {
                    ResetAllIconScales(ctx->icons);
                    ctx->isInitialized = false;
                    ctx->icons.clear();
                }
            } catch (...) {}
        };
        RunFromWindowThread(
            hWnd,
            [](PVOID p) {
                auto* fn = static_cast<std::function<void()>*>(p);
                (*fn)();
            },
            &action);
    }
    g_isBouncing = false;

    TaskbarAutoHide::SettingsChanged(bReload);

    return TRUE;
}