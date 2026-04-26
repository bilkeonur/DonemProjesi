using Microsoft.AspNetCore.Mvc;
using NidsBackendAPI.Models;

namespace NidsBackendAPI.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class SensorController : ControllerBase
    {
        [HttpPost("temperature")]
        public IActionResult ReceiveTemperature([FromBody] SensorData data)
        {
            if (data == null || string.IsNullOrEmpty(data.DeviceId))
            {
                return BadRequest(new { message = "Geçersiz veri formatı." });
            }

            Console.WriteLine($"[{DateTime.Now:HH:mm:ss}] NIDS Veri Akışı -> Cihaz: {data.DeviceId} | Sıcaklık: {data.Temperature}°C");

            return Ok(new { 
                message = "Sıcaklık verisi NIDS sistemine başarıyla ulaştı.", 
                timestamp = DateTime.Now 
            });
        }
    }
}