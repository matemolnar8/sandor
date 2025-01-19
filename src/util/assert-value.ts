export function assert(condition: unknown, message: string): asserts condition {
    if (!condition) {
        throw new Error(message);
    }
}

export function assertAndGet<T>(value: T | null | undefined, message: string): T {
    assert(value != null, message);
    return value;
}