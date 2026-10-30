> Momentum Preview 2: download **SpidyVR-Webwing-Momentum-Win64.zip**. [Yeni kontroller ve ayarlar](TRAVERSAL-FLOW.md). Previous installation instructions are preserved below.

## Türkçe kurulum ve kullanım

### 1. Gerekenler
- Windows bilgisayarında **Marvel’s Spider-Man Remastered PC** kurulu olmalı. Oyunun kendisi bu pakette bulunmaz.
- PC’ye bağlı bir VR başlığı ve başlığına uygun çalışan **OpenXR** ortamı gerekir. Başlığının bağlantı uygulamasını (örneğin SteamVR, Meta Quest Link veya kullandığın bağlantı yazılımı) önceden aç.

### 2. Modu indir
[Yayın sayfasındaki](https://github.com/dincelerenberk-svg/SpidyVR-Webwing/releases/tag/v0.4.3-webwing-preview.1) **Assets** bölümünden **SpidyVR-Webwing-Win64.zip** dosyasını indir. **Source.zip** geliştiriciler içindir; oynamak için gerekli değildir.

### 3. ZIP’in tamamını çıkart
İndirdiğin ZIP’e sağ tıkla ve **Tümünü ayıkla** seçeneğini kullan. Yeni bir klasöre çıkart; ZIP’in içinden çalıştırma. Bütün alt klasörleri ve dosyaları birlikte tut.

**Bu VR paketini oyunun kurulu olduğu klasöre kopyalaman gerekmez.** Örneğin Belgeler altında ayrı bir SpidyVR-Webwing klasöründe tutabilirsin.

### 4. VR modunu başlat
1. VR başlığını bilgisayara bağla ve bağlantı uygulamasının başlığı gördüğünü kontrol et.
2. Çıkarttığın mod klasöründeki **Spidy Launcher.exe** dosyasını aç.
3. Launcher’ın gösterdiği oyun ve VR ortamı kontrollerini incele. Hata varsa önce ekrandaki açıklamaya göre çöz.
4. **START VR** düğmesine bas.
5. Oyun açılınca kayıtlı oyununu yükle / **Continue** seçeneğiyle devam et.
6. Mevcut VR seçeneklerini oyunda **Settings → SPIDY VR** menüsünden ayarlayabilirsin.

### 5. Glider / süzülme
Bu pakette glider ayarı zaten bulunur: **build/windows-ninja/spidy-glider.ini**. Dosya **spidy_ray_bridge.dll** ile aynı klasörde olmalı ve **Enabled=1** yazmalı. Glider için SPIDY VR menüsünde ayrı bir seçenek yoktur.

1. Ağla sallanarak ileri hız kazan.
2. İki ağı da bırak; aşağı süzülmeye başladığında iki kolunu omuz hizasında yana aç.
3. Ellerini yaklaşık **1,05 metre veya daha fazla** ayır. Grip ve trigger tuşlarını bırakıp bu duruşu en az **0,2 saniye** tut.
4. Bakışın ve hareket çubuğuyla yön ver.
5. Kollarını yaklaştırarak/indirerek veya ağ tuşuna basarak süzülmeyi kapat.

Glider yalnızca ağ hareket sisteminin zaten kontrol ettiği uçuşta, ağdan ayrıldıktan sonra çalışır; normal zıplama veya oyunun kendi serbest düşüşünü devralmaz. İleri momentum gerekir; çok düşük hızda açılmaz. **Görünür kanat modeli yoktur; eklenen özellik süzülme fiziğidir.**

### 6. Ayarlar ve sorun giderme
- Glider’ı kapatmak için INI dosyasındaki **Enabled=1** satırını **Enabled=0** yap ve VR oturumunu yeniden başlat.
- **SinkSpeed** iniş hızını (2–12 m/s), **LiftAcceleration** düşüşten toparlamayı (4–24 m/s²) ayarlar. Varsayılanlar 4 ve 14’tür.
- Glider açılmıyorsa önce ağla hız kazanıp ağları tamamen bıraktığını, iki elin de takip edildiğini ve grip/trigger tuşlarına basmadığını kontrol et.
- **Launcher’daki standart güncelleme işlemi bu özel modun dosyalarını upstream sürümle değiştirebilir.** Webwing paketini ayrı klasörde tut.
- **Türkçe altyazı yaması ayrı kurulur.** Aşağıdaki Calypso bağlantısında kendi kurulum talimatları vardır; VR moduyla birlikte uyumluluğu test edilmedi. Türkçe dublaj pakete dahil değildir.

**Deneysel ön sürüm:** Derleme ve otomatik testler başarılıdır; gerçek oyunda ve VR başlığında henüz denenmemiştir.

---

