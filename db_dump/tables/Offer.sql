CREATE TABLE IF NOT EXISTS Offer
(
id INT8 NOT NULL,
guid VARCHAR(64),
campaignId VARCHAR(64),
accountId VARCHAR(64),
retargetingId INT8,
UNIQUE (id) ON CONFLICT IGNORE,
UNIQUE (guid) ON CONFLICT IGNORE,
UNIQUE (accountId,retargetingId) ON CONFLICT IGNORE
);

CREATE INDEX IF NOT EXISTS idx_Offer_id ON Offer (id);
CREATE INDEX IF NOT EXISTS idx_Campaign_id ON Offer (campaignId);
CREATE INDEX IF NOT EXISTS idx_Offer_id_accountId_retargetingId ON Offer (id,accountId,retargetingId);
