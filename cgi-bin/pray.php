<?php
// Setting header for CGI
header("Content-Type: text/html; charset=UTF-8");

// Fetch the city name from the URL (default to "Casablanca" if not provided)
$city = isset($_GET['city']) ? $_GET['city'] : 'Casablanca';
$country = 'Morocco';

// Prepare the URL for API request
$apiUrl = "https://api.aladhan.com/v1/timingsByCity?city=" . urlencode($city) . "&country=" . urlencode($country) . "&method=2";

// Fetch prayer times data from API
$response = file_get_contents($apiUrl);
$data = json_decode($response, true);

// Check if data was fetched successfully
if ($data['code'] == 200) {
    $prayerTimes = $data['data']['timings'];
    $date = $data['data']['date']['gregorian'];
} else {
    echo "<p>Error fetching prayer times. Please try again later.</p>";
    exit();
}

// Get current time in the city for comparison (we'll use UTC for simplicity)
date_default_timezone_set('UTC');
$currentTime = new DateTime();
$prayerTimesFormatted = [];
foreach ($prayerTimes as $prayer => $time) {
    $prayerTimesFormatted[$prayer] = DateTime::createFromFormat('H:i', $time);
}

// Find the next prayer
$nextPrayer = null;
$timeRemaining = null;
foreach ($prayerTimesFormatted as $prayer => $time) {
    if ($currentTime < $time) {
        $nextPrayer = $prayer;
        $timeRemaining = $currentTime->diff($time);
        break;
    }
}

if (!$nextPrayer) {
    // If no more prayers today, reset to Fajr of the next day
    $nextPrayer = 'Fajr';
    $timeRemaining = $currentTime->diff($prayerTimesFormatted['Fajr']);
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Prayer Times for <?php echo htmlspecialchars($city); ?>, Morocco</title>
    
    <!-- Embed CSS directly in the PHP file -->
    <style>
        /* General Styles */
        body {
            font-family: 'Arial', sans-serif;
            background: url('https://img.freepik.com/free-photo/hassan-ii-mosque-surrounded-by-water-buildings-blue-sky-sunlight_181624-26376.jpg') no-repeat center center fixed;
            background-size: cover;
            color: #ffffff;
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            overflow: hidden;
        }

        .container {
            width: 90%;
            max-width: 900px;
            background-color: rgba(0, 0, 0, 0.7);
            border-radius: 15px;
            box-shadow: 0 0 20px rgba(0, 0, 0, 0.3);
            padding: 30px;
            animation: fadeIn 1s ease-in-out;
        }

        @keyframes fadeIn {
            from {
                opacity: 0;
            }
            to {
                opacity: 1;
            }
        }

        header {
            text-align: center;
            margin-bottom: 30px;
        }

        header h1 {
            font-size: 2.5rem;
            color: #f1c40f;
            text-transform: uppercase;
            margin-bottom: 20px;
            letter-spacing: 3px;
            animation: slideIn 1.5s ease-out;
        }

        @keyframes slideIn {
            from {
                transform: translateY(-50px);
                opacity: 0;
            }
            to {
                transform: translateY(0);
                opacity: 1;
            }
        }

        select {
            padding: 12px;
            font-size: 1rem;
            border: none;
            border-radius: 8px;
            background-color: #f1c40f;
            color: #2c3e50;
            margin-top: 20px;
            cursor: pointer;
            animation: bounce 1s ease infinite;
        }

        @keyframes bounce {
            0%, 100% {
                transform: translateY(0);
            }
            50% {
                transform: translateY(-10px);
            }
        }

        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 30px;
            animation: fadeIn 2s ease-in-out;
        }

        table th, table td {
            padding: 15px;
            text-align: center;
            font-size: 1.1rem;
        }

        table th {
            background-color: #16a085;
            color: white;
            text-transform: uppercase;
        }

        table tr:hover {
            background-color: #2ecc71;
            transform: scale(1.02);
            transition: all 0.3s ease-in-out;
        }

        .countdown {
            font-size: 1.4rem;
            font-weight: bold;
            color: #e74c3c;
            margin-top: 20px;
            text-align: center;
        }

        .masjid-img {
            width: 100%;
            height: auto;
            border-radius: 15px;
            margin-top: 20px;
            animation: zoomIn 2s ease-in-out;
        }

        @keyframes zoomIn {
            from {
                transform: scale(0.8);
                opacity: 0;
            }
            to {
                transform: scale(1);
                opacity: 1;
            }
        }

        footer {
            text-align: center;
            margin-top: 30px;
            font-size: 0.9rem;
            color: #ecf0f1;
        }

        footer a {
            color: #f1c40f;
            text-decoration: none;
        }

        footer a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>Prayer Times for <?php echo htmlspecialchars($city); ?>, Morocco</h1>

            <!-- City Select Dropdown -->
            <form method="get" action="" style="text-align: center;">
                <select name="city" onchange="this.form.submit()">
                    <option value="Casablanca" <?php echo $city === 'Casablanca' ? 'selected' : ''; ?>>Casablanca</option>
                    <option value="Rabat" <?php echo $city === 'Rabat' ? 'selected' : ''; ?>>Rabat</option>
                    <option value="Marrakech" <?php echo $city === 'Marrakech' ? 'selected' : ''; ?>>Marrakech</option>
                    <option value="Fes" <?php echo $city === 'Fes' ? 'selected' : ''; ?>>Fes</option>
                    <option value="Tangier" <?php echo $city === 'Tangier' ? 'selected' : ''; ?>>Tangier</option>
                    <option value="Agadir" <?php echo $city === 'Agadir' ? 'selected' : ''; ?>>Agadir</option>
                    <option value="Tetouan" <?php echo $city === 'Tetouan' ? 'selected' : ''; ?>>Tetouan</option>
                    <option value="Oujda" <?php echo $city === 'Oujda' ? 'selected' : ''; ?>>Oujda</option>
                    <option value="Meknes" <?php echo $city === 'Meknes' ? 'selected' : ''; ?>>Meknes</option>
                    <option value="Kenitra" <?php echo $city === 'Kenitra' ? 'selected' : ''; ?>>Kenitra</option>
                    <option value="El Jadida" <?php echo $city === 'El Jadida' ? 'selected' : ''; ?>>El Jadida</option>
                    <option value="Safi" <?php echo $city === 'Safi' ? 'selected' : ''; ?>>Safi</option>
                    <option value="Nador" <?php echo $city === 'Nador' ? 'selected' : ''; ?>>Nador</option>
                    <!-- Add more cities if needed -->
                </select>
            </form>
        </header>

        <div class="countdown">
            <!-- Display countdown to the next prayer -->
            <p>Next Prayer: <strong><?php echo ucfirst($nextPrayer); ?></strong></p>
            <p>Time Remaining: <strong><?php echo $timeRemaining->format('%h hours %i minutes'); ?></strong></p>
        </div>

        <table class="prayer-table">
            <thead>
                <tr>
                    <th>Prayer</th>
                    <th>Time</th>
                </tr>
            </thead>
            <tbody>
                <?php
                // Display the prayer times
                foreach ($prayerTimes as $prayer => $time) {
                    echo "<tr><td>" . ucfirst($prayer) . "</td><td>" . $time . "</td></tr>";
                }
                ?>
            </tbody>
        </table>

        <!-- Mosque Image Below the Prayer Times -->
        <img src="https://img.freepik.com/free-photo/hassan-ii-mosque-surrounded-by-water-buildings-blue-sky-sunlight_181624-26376.jpg" alt="Masjid Image" class="masjid-img">

        <footer>
            <p>Data provided by <a href="https://aladhan.com" target="_blank">Aladhan API</a>.</p>
        </footer>
    </div>
</body>
</html>
