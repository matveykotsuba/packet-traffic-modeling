from __future__ import annotations

import argparse
import csv
from pathlib import Path
from typing import Dict, List, Optional, Sequence, Tuple

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


PacketRow = Tuple[float, float]


def read_metadata(path: Optional[Path]) -> Dict[str, str]:
    if path is None or not path.exists():
        return {}

    metadata: Dict[str, str] = {}
    for line in path.read_text(encoding="utf-8-sig").splitlines():
        line = line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, value = line.split("=", 1)
        metadata[key.strip()] = value.strip()
    return metadata


def parse_packet_row(row: Sequence[str], line_number: int) -> PacketRow:
    if len(row) != 2:
        raise ValueError(f"CSV line {line_number}: expected 2 columns, got {len(row)}")

    try:
        arrival_time = float(row[0])
        packet_size = float(row[1])
    except ValueError as error:
        raise ValueError(f"CSV line {line_number}: time and size must be numbers") from error

    if arrival_time < 0.0:
        raise ValueError(f"CSV line {line_number}: packet arrival time cannot be negative")
    if packet_size <= 0.0:
        raise ValueError(f"CSV line {line_number}: packet size must be positive")

    return arrival_time, packet_size


def read_traffic_csv(path: Path) -> List[PacketRow]:
    if not path.exists():
        raise FileNotFoundError(f"CSV file does not exist: {path}")

    rows: List[PacketRow] = []
    with path.open("r", encoding="utf-8-sig", newline="") as input_file:
        reader = csv.reader(input_file)
        first_row = next(reader, None)
        if first_row is None:
            return rows

        normalized_header = [value.strip().lower() for value in first_row]
        if normalized_header != ["time", "size"]:
            rows.append(parse_packet_row(first_row, 1))

        for line_number, row in enumerate(reader, start=2):
            if not row or all(not value.strip() for value in row):
                continue
            rows.append(parse_packet_row(row, line_number))

    rows.sort(key=lambda packet: packet[0])
    return rows


def calculate_delays(times: Sequence[float]) -> List[float]:
    return [times[index] - times[index - 1] for index in range(1, len(times))]


def calculate_average_bitrate(packet_sizes: Sequence[float], times: Sequence[float], simulation_time: Optional[float]) -> float:
    total_bits = sum(packet_sizes) * 8.0

    if simulation_time is not None and simulation_time > 0.0:
        duration = simulation_time
    elif len(times) >= 2:
        duration = times[-1] - times[0]
    else:
        duration = 0.0

    if duration <= 0.0:
        return 0.0

    return total_bits / duration


def save_histogram(values: Sequence[float], title: str, xlabel: str, output_path: Path) -> None:
    plt.figure(figsize=(7, 4))
    if len(values) == 0:
        plt.text(0.5, 0.5, "No data", ha="center", va="center")
        plt.title(title)
        plt.xlabel(xlabel)
        plt.ylabel("count")
    else:
        plt.hist(values, bins="auto", edgecolor="black")
        plt.title(title)
        plt.xlabel(xlabel)
        plt.ylabel("count")
    plt.tight_layout()
    plt.savefig(output_path)
    plt.close()


def write_statistics(path: Path, lines: Sequence[str]) -> None:
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Analyze generated traffic CSV")
    parser.add_argument("--csv", required=True, type=Path, help="Path to generated traffic CSV")
    parser.add_argument("--metadata", type=Path, default=None, help="Path to metadata file")
    parser.add_argument("--output-dir", type=Path, default=Path("output"), help="Directory for plots and statistics")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)

    metadata = read_metadata(args.metadata)
    packets = read_traffic_csv(args.csv)

    model_name = metadata.get("model", "unknown")
    simulation_time_raw = metadata.get("simulation_time_seconds")
    simulation_time = float(simulation_time_raw) if simulation_time_raw else None

    times = [packet[0] for packet in packets]
    sizes = [packet[1] for packet in packets]
    delays = calculate_delays(times)

    packets_count = len(packets)
    average_packet_size = sum(sizes) / packets_count if packets_count > 0 else 0.0
    average_delay = sum(delays) / len(delays) if delays else 0.0
    average_bitrate = calculate_average_bitrate(sizes, times, simulation_time)

    delay_plot = args.output_dir / "delay_histogram.png"
    size_plot = args.output_dir / "packet_size_histogram.png"
    stats_file = args.output_dir / "statistics.txt"

    save_histogram(delays, "Packet delay histogram", "delay between packets, s", delay_plot)
    save_histogram(sizes, "Packet size histogram", "packet size, bytes", size_plot)

    lines = [
        f"Traffic model: {model_name}",
        f"Packets count: {packets_count}",
        f"Average packet size: {average_packet_size:.6f} bytes",
        f"Average bitrate: {average_bitrate:.6f} bit/s",
        f"Average delay between packets: {average_delay:.6f} s",
        f"Delay histogram: {delay_plot}",
        f"Packet size histogram: {size_plot}",
    ]

    write_statistics(stats_file, lines)

    for line in lines:
        print(line)
    print(f"Statistics saved to: {stats_file}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
