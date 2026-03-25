# Substituição unificada dos patches Narco (torna os scripts antigos obsoletos)

Este pacote substitui os patches isolados por um conjunto coeso de scripts:

- evento global de captura de base
- atualização de cache e reconciliação de rádio
- componente de conexão direta entre bases
- refresh de UI do mapa quando captura acontece com o mapa aberto
- hotfix de null-check em spawn point de mapa

## Arquivos

- `Scripts/Game/Campaign/Narco_CampaignBaseEvents.c`
- `Scripts/Game/Campaign/Narco_DirectConnectBaseComponent.c`
- `Scripts/Game/Campaign/Narco_CacheSystem.c`
- `Scripts/Game/Campaign/Narco_RadioCoverageSystem.c`
- `Scripts/Game/Campaign/Narco_CoverageRadioComponent.c`
- `Scripts/Game/UI/Narco_MapEventSystem.c`
- `Scripts/Game/UI/Narco_MapUISpawnPoint.c`

## Ordem de funcionamento

1. `SCR_CampaignMilitaryBaseManager.OnAllBasesInitialized()` monta cache (`Narco_CacheSystem`) e inicia rádio (`Narco_InitAndStartUpdates`).
2. Mudanças de facção em base disparam invalidação de capturas sem rota HQ (`OnBaseFactionChanged`).
3. `SCR_CampaignMilitaryBaseComponent.OnFactionChanged()` publica evento global `OnBaseCaptured`.
4. `SCR_MapEntity` escuta esse evento durante mapa aberto e força redraw limpo do mapa.
5. `SCR_RadioCoverageSystem` reconstrói grafo simétrico de rádios e recalcula cobertura.

## Resultado

Com isso, os snippets antigos separados ficam desnecessários: a lógica foi consolidada em arquivos únicos por domínio (Campaign/UI), com cache e eventos explícitos.
