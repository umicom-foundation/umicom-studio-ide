<!-- Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT -->

# Android Templates

## Full Multi-Module

- Location: `templates/android/full-mm`
- Contains: `app/`, `feature/`, `lib/`, `gradle/`, wrapper scripts

### Build

```bash
cd templates/android/full-mm
./scripts/fetch_gradle_wrapper.sh  # or .ps1 on Windows
./gradlew assembleDebug
```

## Enterprise (Flavors + Hilt + CI)

- Location: `templates/android/enterprise`
- Try: `./gradlew :app:assembleFreeDebug`
