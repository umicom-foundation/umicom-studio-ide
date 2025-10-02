// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
const std = @import("std");
pub fn main() !void {
    try std.io.getStdOut().writer().print("Hello from Zig in Umicom Studio IDE!\n", .{});
}
