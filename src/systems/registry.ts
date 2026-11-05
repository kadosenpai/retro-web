import { gba } from './gba';
import type { SystemProfile } from './types';

const all: SystemProfile[] = [gba];

const byId = new Map<string, SystemProfile>(all.map((s) => [s.id, s]));
const byExtension = new Map<string, SystemProfile>();
for (const system of all) {
  for (const ext of system.extensions) byExtension.set(ext, system);
}

export function listSystems(): readonly SystemProfile[] {
  return all;
}

export function getSystem(id: string): SystemProfile | undefined {
  return byId.get(id);
}

/** Nhận diện hệ máy từ tên file, ví dụ "pokemon.gba" → GBA. */
export function getSystemByFileName(fileName: string): SystemProfile | undefined {
  const dot = fileName.lastIndexOf('.');
  if (dot < 0) return undefined;
  return byExtension.get(fileName.slice(dot + 1).toLowerCase());
}
