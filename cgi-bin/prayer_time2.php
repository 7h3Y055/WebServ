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
            font-family: Arial, sans-serif;
            background-color: #f4f7fb;
            color: #333;
            margin: 0;
            padding: 0;
        }

        .container {
            width: 90%;
            max-width: 900px;
            margin: 20px auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 10px;
            box-shadow: 0 0 15px rgba(0, 0, 0, 0.1);
        }

        header {
            text-align: center;
            margin-bottom: 20px;
        }

        header h1 {
            font-size: 2rem;
            color: #1f8c7d;
        }

        select {
            padding: 10px;
            font-size: 1rem;
            border: 1px solid #ddd;
            border-radius: 5px;
            background-color: #fff;
            margin-top: 10px;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
        }

        table th, table td {
            padding: 12px;
            text-align: center;
            border-bottom: 1px solid #ddd;
            font-size: 1rem;
        }

        table th {
            background-color: #1f8c7d;
            color: white;
        }

        table tr:hover {
            background-color: #f1f1f1;
        }

        .countdown {
            font-size: 1.2rem;
            font-weight: bold;
            color: #ff5722;
            margin-top: 15px;
            text-align: center;
        }

        footer {
            text-align: center;
            margin-top: 30px;
            font-size: 0.9rem;
        }

        footer a {
            color: #1f8c7d;
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
                    <!-- Add more cities as needed -->
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

        <footer>
            <p>Data provided by <a href="https://aladhan.com" target="_blank">Aladhan API</a>.</p>
        </footer>
    </div>
</body>
</html>
