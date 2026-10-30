# Webwing Momentum Preview 2 — Türkçe kullanım ve gerçek kapsam

Bu deneysel paket 0.4.3 tabanındadır. Upstream 0.6.0'a geçirilmemiştir.
Kaynak ve Windows derlemesi kontrol edildi; gerçek oyun/VR başlığı testi yapılmadı.
Tam fizik sandbox veya bütün fikirlerin tamamlandığı bir sürüm değildir.

## Bu sürümde yeni eklenenler

- Kontrollü dalış: İki elini başına göre gövdene yakın tut, eller arasını 60 cm'den az aç ve aşağı bak. Düşey hızın -3 m/sn'nin altındayken ilave 5 m/sn² dalış ivmesi uygulanır. Açık kollu glider pozuyla karışmaz.
- Havada yönlendirme: Serbest uçuşta çubuğu gitmek istediğin dünya yönüne ver. Yatay momentum, saniyede en fazla 0,6 radyan döndürülür; bu işlem kendi başına hız üretmez.
- Ağ geçişi zinciri: Gerçekten tutunmuş ağlar arasında el değiştir. En az 0,45 saniye ve çözücü hızından hesaplanan 6 metre hareket gerekir. İlk tutunma bonus vermez. Zincir en fazla 5 basamaktır; her geçiş 0,6 × basamak m/sn ekler. 4 saniye geçiş olmazsa zincir söner. Tetik spam'i veya aynı ağı tutmaya devam etmek bonus vermez.
- Duvar sıçraması desteği: Mevcut duvar sıçrama hareketine, hareket yönünde 1,2 m/sn ek destek. Bu yeni bir duvar tutunma algoritması değildir.

Bu ilaveler mevcut hız ayarını aşmaz; motorun üst sınırı 65 m/sn'dir (234 km/sa). Gerçek Mach 1 hızı veya ses duvarı simülasyonu yoktur. Oyun çarpışmaları konum üzerinde yetkilidir.
Uçuş eklemeleri yalnızca modun hareket kontrolünü zaten aldığı ağ uçuşunda çalışır. Normal oyunun her serbest düşüşünü devralmaz.
Odak/takip kaybı, iniş, duvar tutunması ve başka sistemin hareketi devralması zinciri sıfırlar.
Duvar desteği odak ve iki el takibi gerektirir; nesne tutulurken uygulanmaz.
Kafa kamerasına yapay G-kuvveti sarsıntısı uygulanmaz.

## Kurulum ve kapatma

1. Bu sürümün **SpidyVR-Webwing-Momentum-Win64.zip** dosyasını indir; kaynak kod ZIP'i oyuncu paketi değildir.
2. Yeni, ayrı bir klasöre tamamen çıkar. Eski paketin üzerine yazmak zorunda değilsin.
3. İçindeki **Spidy Launcher.exe** ile başlat. Ayrıntılar: [Türkçe kurulum](TURKCE-KURULUM.md).
4. `build/windows-ninja/spidy-glider.ini` dosyasında `[Flow]` bölümü pakette açıktır. Değişiklikten sonra modu yeniden başlat.

```ini
[Flow]
Enabled=1
DiveAcceleration=5
TurnRatePercent=60
TransferBoostPercent=60
```

`Enabled=0` yeni dört hareket desteğini kapatır. Glider'ın `[Glider]` ayarı ayrıdır.
Dalış ivmesi 0–12 m/sn²; dönüş ayarı 0–200 (100 = 1 rad/sn); geçiş ayarı 0–200 (100 = basamak başına 1 m/sn). Duvar desteği geçiş değerinin iki katıdır.
INI yoksa yeni hareket desteği varsayılan olarak kapalıdır.

## İstenen diğer fikirlerin durumu

| Fikir | Bu paketteki gerçek durum |
| --- | --- |
| Çift ağ sapanı, kesintisiz ağ değiştirme | Önceden mevcut; yeni fizik motoru olarak sunulmuyor. |
| Nesne ve düşman fırlatma, düşman çarpışma hasarı | Önceden mevcut; sadece oyunun onaylı taşınabilir nesneleri ve desteklenen düşmanları. Her nesne veya yaya tutulamaz. |
| Elektrik ağı, gadget çarkı, savaş drone'u | Önceden mevcut. Savaş drone'u serbest kamera drone'u değildir. |
| Kollarla glider | Önceki ön sürümdeki isteğe bağlı glider korunur. Görünür kanat modeli yoktur. |
| Her nesneyi yakın dövüş silahına çevirme, gelişmiş ragdoll | Eklenmedi; hedef/hasar ve iskelet bağlantıları ayrıca doğrulanmalı. |
| Cam, korkuluk, kıyafet parçalama | Eklenmedi; kırılma varlıkları ve motor bağlantıları gerektirir. |
| Üzerinde yürünebilen manuel ağ | Eklenmedi; görsel ip çizmek yürünebilir çarpışma yüzeyi oluşturmaz. |
| Elle yangın merdiveni/pencere tutma, gap dash, yer kayması, otomatik zip zinciri | Eklenmedi; geometri doğrulaması ve native hareket geçişleri gerekir. Duvar veya objelerin içinden geçirme yoktur. |
| Büyük kafa, komik poz/sesler, köpük ve mıknatıs ağı | Eklenmedi; yeni iskelet/gadget/varlık desteği gerekir. |
| Kumandalı dış kamera drone'u | Eklenmedi; bağımsız render kamerası ve kontroller gerekir. |
| Rüzgâr sesi, hız çizgileri/tüneli, sonic boom, trafik sıyırma bonusu | Bu sürümde eklenmedi. Yakın geçiş veya ses duvarı yaşanmış gibi sahte olay üretilmez. |

## Doğrulama

Windows Release derlemesi; CTest'in beş test grubu; 133 Python protokol testi.
Yeni hareket testleri: varsayılan kapalı davranış, dalış pozu, momentum koruyan dönüş, ilk tutunma, geçiş/tekrar tetikleme, zincir süresi, odak/takip kaybı, hız sınırı, geçersiz süre/ayar, duvar desteği.
`tools/audit-additions.py` özgün tabandaki satırların silinmediğini ve sırasının korunduğunu kontrol eder.
Bu kontroller gerçek oyunda çarpışma, performans veya VR konfor testi yerine geçmez.
