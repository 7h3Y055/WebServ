#!/usr/bin/env python3
import cgi
import cgitb
import requests
from bs4 import BeautifulSoup

# Disable SSL warnings (for requests.get)
import urllib3
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

# Cities dictionary (abbreviated for brevity, full list in the actual script)
CITIES = {
    1: "الرباط",
    2: "الخميسات",
    3: "تيفلت",
    4: "الرماني",
    5: "والماس",
    6: "بوزنيقة",
    7: "القنيطرة",
    8: "سيدي قاسم",
    9: "سيدي يحيى الغرب",
    10: "سيدي سليمان",
    11: "سوق أربعاء الغرب",
    12: "عرباوة",
    13: "مولاي بوسلهام",
    14: "طنجة",
    15: "تطوان",
    16: "العرائش",
    17: "أصيلة",
    18: "شفشاون",
    19: "مرتيل",
    20: "المضيق",
    21: "القصر الكبير",
    22: "القصر الصغير",
    23: "الحسيمة",
    24: "سبتة",
    25: "الفنيدق",
    26: "الجبهة",
    27: "واد لاو",
    28: "باب برد",
    29: "وزان",
    30: "بوسكور",
    31: "وجدة",
    32: "بركان",
    33: "فكيك",
    34: "بوعرفة",
    35: "كرسيف",
    36: "جرادة",
    37: "عين الشعير",
    38: "تاوريرت",
    39: "الناظور",
    40: "مليلية",
    41: "دبدو",
    42: "سلوان",
    43: "بني أنصار",
    44: "فرخانة",
    45: "تالسينت",
    46: "تندرارة",
    47: "العيون الشرقية",
    48: "بني ادرار",
    49: "السعيدية",
    50: "رأس الماء",
    51: "تافوغالت",
    52: "فزوان",
    53: "أحفير",
    54: "زايو",
    55: "دريوش",
    56: "بني تجيت",
    57: "بوعنان",
    58: "الدار البيضاء",
    59: "المحمدية",
    60: "بن سليمان",
    61: "سطات",
    62: "الكارة",
    63: "البروج",
    64: "ابن أحمد",
    65: "برشيد",
    66: "الجديدة",
    67: "أزمور",
    68: "سيدي بنور",
    69: "خميس الزمامرة",
    70: "خنيفرة",
    71: "مولاي بوعزة",
    72: "زاوية أحنصال",
    73: "بني ملال",
    74: "أزيلال",
    75: "الفقيه بنصالح",
    76: "دمنات",
    77: "القصيبة",
    78: "قصبة تادلة",
    79: "خريبكة",
    80: "وادي زم",
    81: "فاس",
    82: "صفرو",
    83: "مولاي يعقوب",
    84: "بولمان",
    85: "ميسور",
    86: "رباط الخير",
    87: "المنزل بني يازغة",
    88: "إموزار كندر",
    89: "تازة",
    90: "تاونات",
    91: "أكنول",
    92: "تيزي وسلي",
    93: "بورد",
    94: "تاهلة",
    95: "تيسة",
    96: "قرية با محمد",
    97: "كتامة",
    98: "واد أمليل",
    99: "مكناس",
    100: "يفرن",
    101: "الحاجب",
    102: "زرهون",
    103: "آزرو",
    104: "مراكش",
    105: "قلعة السراغنة",
    106: "الصويرة",
    107: "شيشاوة",
    108: "بنجرير",
    109: "الرحامنة",
    110: "تمنار",
    111: "آسفي",
    112: "الوليدية",
    113: "اليوسفية",
    114: "تسلطانت",
    115: "تامصلوحت",
    116: "قطارة",
    117: "أكادير",
    118: "تارودانت",
    119: "تزنيت",
    120: "إغرم",
    121: "تالوين",
    122: "تافراوت",
    123: "طاطا",
    124: "أقا",
    125: "فم لحصن",
    126: "بويكرة",
    127: "أولاد تايمة",
    128: "الرشيدية",
    129: "الريصاني",
    130: "أرفود",
    131: "تنديت",
    132: "كلميمة",
    133: "إملشيل",
    134: "تنجداد",
    135: "الريش",
    136: "ميدلت",
    137: "زاكورة",
    138: "ورزازات",
    139: "تنغير",
    140: "هسكورة",
    141: "قلعة مكونة",
    142: "أكدز",
    143: "بومالن دادس",
    144: "النيف",
    145: "أسول",
    146: "أمسمرير",
    147: "تازارين",
    148: "سيدي إفني",
    149: "كلميم",
    150: "أسا",
    151: "الزاك",
    152: "طانطان",
    153: "بويزكارن",
    154: "المحبس",
    155: "لمسيد",
    156: "العيون",
    157: "السمارة",
    158: "بوجدور",
    159: "طرفاية",
    160: "تفاريتي",
    161: "بوكراع",
    162: "كلتة زمور",
    163: "أمكالة",
    164: "أخفنير",
    165: "الداخلة",
    166: "الكويرة",
    167: "أوسرد",
    168: "بئر كندوز",
    169: "بئر أنزاران",
    301: "خميس سيدي عبد الجليل",
    302: "أولاد عياد",
    303: "تاهلة",
    304: "مطماطة",
    305: "إيمنتانوت",
    306: "سيدي غانم",
    307: "تفنتان",
    308: "آيت القاق",
    309: "أكدال أملشيل",
    310: "اكودال املشيل ميدلت",
    311: "أكايوار",
    312: "عين العودة",
    313: "أسكين",
    314: "آيت ورير",
    315: "زاوية مولاي ابراهيم",
    316: "تولكولت",
    317: "إيكس",
    318: "كرس",
    319: "تيسنت",
    320: "فم زكيد",
    321: "قصر إيش",
    322: "إيمين ثلاث"
}

def get_prayer_times(ville_id):
    """
    Fetch prayer times for a specific city
    
    Args:
        ville_id (int): City ID
    
    Returns:
        dict: Prayer times for the specified city
    """
    # URL and query parameters
    url = "https://www.habous.gov.ma/prieres/horaire-api.php"
    
    params = {"ville": ville_id}

    try:
        # Send GET request with SSL verification disabled
        response = requests.get(url, params=params, verify=False)
        
        # Parse the HTML response
        soup = BeautifulSoup(response.text, "html.parser")
        
        # Locate the table with prayer times
        prayer_times = {}
        table = soup.find("table", class_="horaire")
        
        if table:
            rows = table.find_all("tr")
            
            # Extract times from table rows
            for row in rows:
                columns = row.find_all("td")
                for i in range(0, len(columns), 2):
                    prayer_name = columns[i].text.strip().replace(":", "").strip()
                    prayer_time = columns[i + 1].text.strip()
                    prayer_times[prayer_name] = prayer_time
        
        return prayer_times
    
    except Exception as e:
        return {"Error": str(e)}

def print_html(city_id, city_name, times):
    """Generate HTML page with prayer times"""
    print("Content-Type: text/html; charset=utf-8\n")
    print(f"""
<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <title>أوقات الصلاة - {city_name}</title>
    <style>
        body {{
            font-family: 'Cairo', Arial, sans-serif;
            background-image: url('https://saharaatlastours.com/wp-content/uploads/2019/12/Casablanca-Mosque.jpg');
            background-size: cover;
            background-position: center;
            background-attachment: fixed;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            margin: 0;
            padding: 20px;
            text-align: right;
        }}
        .prayer-container {{
            background: rgba(255, 255, 255, 0.9);
            border-radius: 15px;
            box-shadow: 0 10px 25px rgba(0,0,0,0.2);
            padding: 30px;
            max-width: 500px;
            width: 100%;
        }}
        h1 {{
            color: #2c3e50;
            text-align: center;
            margin-bottom: 20px;
            text-shadow: 1px 1px 2px rgba(0,0,0,0.1);
        }}
        .prayer-times {{
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
        }}
        .prayer-time {{
            background: rgba(241, 241, 241, 0.9);
            border-radius: 10px;
            padding: 15px;
            text-align: center;
            transition: transform 0.3s ease;
        }}
        .prayer-time:hover {{
            transform: scale(1.05);
            background: rgba(230, 230, 230, 0.9);
        }}
        .prayer-name {{
            font-weight: bold;
            color: #34495e;
            margin-bottom: 10px;
        }}
        .time {{
            font-size: 1.2em;
            color: #2c3e50;
        }}
        .city-selector {{
            margin-bottom: 20px;
            text-align: center;
        }}
        select {{
            padding: 10px;
            font-size: 1em;
            border-radius: 5px;
            border: 1px solid #bdc3c7;
        }}
    </style>
</head>
<body>
    <div class="prayer-container">
        <div class="city-selector">
            <form method="get">
                <select name="ville" onchange="this.form.submit()">
                    {"".join(f'<option value="{id}" {"selected" if id == city_id else ""}>{name}</option>' for id, name in CITIES.items())}
                </select>
            </form>
        </div>
        <h1>أوقات الصلاة - {city_name}</h1>
        <div class="prayer-times">
            {"".join([f'''
            <div class="prayer-time">
                <div class="prayer-name">{prayer}</div>
                <div class="time">{time}</div>
            </div>
            ''' for prayer, time in times.items()])}
        </div>
    </div>
</body>
</html>
    """)

def main():
    # Parse the query string
    form = cgi.FieldStorage()
    
    # Get ville (city) parameter
    try:
        ville_id = int(form.getvalue('ville', '1'))
        
        # Validate city ID
        if ville_id < 1 or ville_id > 322:
            ville_id = 1  # Default to Rabat
        
        # Get city name
        city_name = CITIES.get(ville_id, "مدينة غير معروفة")
        
        # Fetch prayer times
        times = get_prayer_times(ville_id)
        
        # Print HTML with prayer times
        print_html(ville_id, city_name, times)
    
    except Exception as e:
        # Error handling
        print("Content-Type: text/html; charset=utf-8\n")
        print(f"<html><body><h1>خطأ</h1><p>{str(e)}</p></body></html>")

if __name__ == "__main__":
    main()